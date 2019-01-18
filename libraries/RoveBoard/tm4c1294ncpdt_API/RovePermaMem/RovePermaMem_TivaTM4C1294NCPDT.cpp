/*
 * RovePermaMem_TivaTM4C1294NCPDT.cpp
 *
 *  Created on: Oct 21, 2017
 *      Author: drue
 */

#include <string.h>
#include <tm4c1294ncpdt_API/RovePermaMem/RovePermaMem_Internal.h>
#include <tm4c1294ncpdt_API/RovePermaMem/RovePermaMem_TivaTM4C1294NCPDT.h>
#include "../tivaware/driverlib/eeprom.h"
#include "../tivaware/driverlib/sysctl.h"
#include "supportingUtilities/Debug.h"

typedef uint32_t word_t;

//blocks 0 to 5 are reserved for roveboard stuff. Block 0 especially, being reserved for this file.
//Users can input 0 to 89, but in reality it gets shifted up to 6 to 95
static const uint8_t BlockIndex_ReservedBlocks = 6;
static const uint8_t BlockIndex_ControlBlock = 0;
static const uint32_t InitFootprintKeyword = 0x0000FEFE;

const uint8_t BlockReferenceTableSize = 3;
const word_t DummyMaskInLastWordOfTable = 0b11111111111111111111111111000000;
const uint32_t ControlBlockPassword = 0x0000EFFE;
const uint8_t WordLengthInBytes = 4;

//block 0 contains all the rovepermamem information. Each block contains 64 bytes, IE 16 words of 4 bytes length.
//First word is reserved to put our initialized footprint into so that we know this eeprom has been initialized before.
//Second, third, and fourth words are all devoted towards keeping track of how many of the blocks are still in their factory state.
//Finally, we keep a table in RAM to remember what blocks have been allocated by app layer functions that request them.
static const uint8_t BlockAddress_ControlBlock = 0;
static const uint8_t ControlBlock_WordOffset_initFootprint = 0 * WordLengthInBytes;
static const uint8_t ControlBlock_WordLength_initFootprint = WordLengthInBytes;
const uint8_t ControlBlock_WordOffset_blockFresh = 1 * WordLengthInBytes; //actually goes from 1 to 3
const uint8_t ControlBlock_WordLength_blockFresh = 3 * WordLengthInBytes;

word_t tm4c1294ncpdt_blockFreshTable[BlockReferenceTableSize];
word_t tm4c1294ncpdt_blockUsedTable[BlockReferenceTableSize];

static void doFirstInit();
static void setupGlobalTables();

static const uint8_t BytesPerBlock = 64;
static const uint8_t TotalBlocks = 96 - BlockIndex_ReservedBlocks;
static const uint8_t bitsPerWord = 32;

static word_t blockCountToBitbandWord(uint16_t x);
static bool isBlockFreshOrUsed(uint16_t blockReference, bool whichToGet);
static uint16_t getTotalUnusedOrFreshBlocks(bool whichToGet);
static uint16_t inputBlockIndexToMemIndex(uint16_t inputBlockIndex);
static void lockEeprom();
static void unlockEeprom();
static void updateUseTable(uint16_t blockReference, bool setUse);
static void updateFreshTable(uint16_t blockReference);

static const bool getFresh = true;
static const bool getUsed = false;

RovePermaMem_Error rovePermaMem_WriteBlockByte(RovePermaMem_Block blockHandle, uint8_t byteReference, uint8_t valueToWrite)
{
  uint16_t blockReference = blockHandle.blockReference;

  if(blockReference >= TotalBlocks || byteReference >= BytesPerBlock)
  {
    return RovePermaMem_InputOutOfBounds;
  }
  else
  {
    bool b;
    rovePermaMem_isBlockUsed(blockReference, &b);
    if(!b)
    {
      return RovePermaMem_BlockNotAllocated;
    }
  }

  uint8_t byteBuff[BytesPerBlock];
  RovePermaMem_Error errVal;

  errVal = rovePermaMem_ReadBlock(blockHandle, byteBuff);
  if(errVal != RovePermaMem_Success)
  {
    return errVal;
  }

  byteBuff[byteReference] = valueToWrite;

  return rovePermaMem_WriteBlock(blockHandle, byteBuff);
}

RovePermaMem_Error rovePermaMem_WriteBlock(RovePermaMem_Block blockHandle, uint8_t bytes[])
{
  uint16_t blockReference = blockHandle.blockReference;
  uint16_t password = blockHandle.password;

  if(blockReference >= TotalBlocks || bytes == 0)
  {
    return RovePermaMem_InputOutOfBounds;
  }
  else
  {
    bool b;
    rovePermaMem_isBlockUsed(blockReference, &b);
    if(!b)
    {
      return RovePermaMem_BlockNotAllocated;
    }
  }

  uint16_t realBlockReference;
  const bool Shitslocked = 0;
  bool isShitStillLocked;

  //update the user's block index to make it fit with the actual EEPROM block mapping
  realBlockReference = inputBlockIndexToMemIndex(blockReference);

  unlockEeprom();
  isShitStillLocked = EEPROMBlockUnlock(realBlockReference, (uint32_t*)&password, 1);
  if(isShitStillLocked == Shitslocked)
  {
    return RovePermaMem_ImproperPassword;
  }

  EEPROMProgram((uint32_t*)bytes, EEPROMAddrFromBlock(realBlockReference), BytesPerBlock);

  EEPROMBlockLock(realBlockReference);
  lockEeprom();

  return RovePermaMem_Success;
}

RovePermaMem_Error rovePermaMem_ReadBlockByte(RovePermaMem_Block blockHandle, uint8_t byteReference, uint8_t *readBuffer)
{
  uint16_t blockReference = blockHandle.blockReference;

  if(blockReference >= TotalBlocks || byteReference >= BytesPerBlock || readBuffer == 0)
  {
    return RovePermaMem_InputOutOfBounds;
  }
  else
  {
    bool b;
    rovePermaMem_isBlockUsed(blockReference, &b);
    if(!b)
    {
      return RovePermaMem_BlockNotAllocated;
    }
  }

  uint8_t byteBuff[BytesPerBlock];
  RovePermaMem_Error errVal;

  errVal = rovePermaMem_ReadBlock(blockHandle, byteBuff);
  if(errVal != RovePermaMem_Success)
  {
    return errVal;
  }

  *readBuffer = byteBuff[byteReference];

  return RovePermaMem_Success;
}

RovePermaMem_Error rovePermaMem_ReadBlock(RovePermaMem_Block blockHandle, uint8_t byteBuffer[])
{
  uint16_t blockReference = blockHandle.blockReference;
  uint16_t password = blockHandle.password;

  if(blockReference >= TotalBlocks || byteBuffer == 0)
  {
    return RovePermaMem_InputOutOfBounds;
  }
  else
  {
    bool b;
    rovePermaMem_isBlockUsed(blockReference, &b);
    if(!b)
    {
     return RovePermaMem_BlockNotAllocated;
    }
  }

  uint16_t realBlockReference;
  const bool Shitslocked = 0;
  bool isShitStillLocked;

  //update the user's block index to make it fit with the actual EEPROM block mapping
  realBlockReference = inputBlockIndexToMemIndex(blockReference);

  unlockEeprom();
  isShitStillLocked = EEPROMBlockUnlock(realBlockReference, (uint32_t*)&password, 1);
  if(isShitStillLocked == Shitslocked)
  {
   return RovePermaMem_ImproperPassword;
  }

  EEPROMRead(((uint32_t*)byteBuffer), EEPROMAddrFromBlock(realBlockReference), BytesPerBlock);

  EEPROMBlockLock(realBlockReference);
  lockEeprom();

  return RovePermaMem_Success;
}

RovePermaMem_Block rovePermaMem_useBlock(uint16_t blockReference, uint16_t passwordToUse)
{
  if(blockReference >= TotalBlocks)
  {
    debugFault("useBlock: block reference out of bounds");
  }
  else
  {
    bool b;
    rovePermaMem_isBlockUsed(blockReference, &b);
    if(b)
    {
      debugFault("useBlock: block already used by another process");
    }
  }

  //inputs having been checked, reserve the block requested by unlocking the EEPROM module
  //and setting the hardware to use the user's password whenever that block is used
  uint32_t writeVal;
  uint16_t realBlockReference;

  //update the user's block index to make it fit with the actual EEPROM block mapping
  realBlockReference = inputBlockIndexToMemIndex(blockReference);

  //begin writing things to eeprom
  unlockEeprom();

  writeVal = passwordToUse;
  EEPROMBlockPasswordSet(EEPROMAddrFromBlock(realBlockReference), &writeVal, 1);
  EEPROMBlockLock(realBlockReference);

  //update tables. Eeprom remains unlocked so the functions can update eeprom if need be
  updateUseTable(blockReference, true);
  updateFreshTable(blockReference);

  lockEeprom();

  RovePermaMem_Block block;
  block.blockReference = blockReference;
  block.password = passwordToUse;

  return block;
}


bool rovePermaMem_getFirstAvailableBlock(bool onlyGetFreshBlocks, uint16_t startingBlock, uint16_t *ret_blockReference)
{
  if(startingBlock >= TotalBlocks)
  {
    return false;
  }

  const uint16_t initValue = 0xFFFF; //way beyond what the size can actually get to, so serves as a good 'hasn't been set yet' value
  int i, j;
  int i_start;
  int j_start;

  word_t bitBand;
  *ret_blockReference = initValue;

  word_t blockUnusedTable[BlockReferenceTableSize];

  //tm4c1294ncpdt_blockUsedTable has a 1 for every used and 0 for unused, so invert it to get an unused table.
  //while we're at it, let's save ourselves another for loop and initialize j_start
  for(i = 0; i < BlockReferenceTableSize; i++)
  {
    blockUnusedTable[i] = ~(tm4c1294ncpdt_blockUsedTable[i]);
  }

  //tm4c1294ncpdt_blockUsedTable lists a 0 for all dummy values which get turned into 1 for the unused table.
  //set them back to 0 to make it look like they're used so the search algorithm won't select them
  blockUnusedTable[BlockReferenceTableSize - 1] &= DummyMaskInLastWordOfTable;

  //figure out where in the search algorithm we want to start at.
  if(startingBlock < bitsPerWord)
  {
    i_start = 0;
    j_start = startingBlock;
  }
  else if(startingBlock >= bitsPerWord * 2)
  {
    i_start = 2;
    j_start = startingBlock - bitsPerWord * i_start;
  }
  else
  {
    i_start = 1;
    j_start = startingBlock - bitsPerWord * i_start;
  }

  //90 total available blocks, but kept in an array of 32 bits. So, search through the array by checking
  // each of the 32 bits in each of its indexes.
  for(i = i_start; i < BlockReferenceTableSize; i++)
  {
    for(j = j_start; j < bitsPerWord; j++)
    {
      bitBand = blockCountToBitbandWord(j);
      if(bitBand & blockUnusedTable[i])
      {
        if(!onlyGetFreshBlocks || (bitBand & tm4c1294ncpdt_blockFreshTable[i]))
        {
          *ret_blockReference = j + i * bitsPerWord;
          break;
        }
      }
    }

    //check to see if we found anything yet. If so, go ahead and exit loop
    if(*ret_blockReference != initValue)
    {
      break;
    }

    //j_start needs to be 0 for every loop except for the starting point so that it knows where to start
    //but afterwards won't start skipping numbers for all iterations of i after
    j_start = 0;
  }

  if(*ret_blockReference != initValue)
  {
    return true;
  }
  else
  {
    return false;
  }
}

uint16_t rovePermaMem_getTotalUsedBlocks()
{
  return getTotalUnusedOrFreshBlocks(getUsed);
}

uint16_t rovePermaMem_getTotalUnusedBlocks()
{
  //Incidentally, it's easiest to only do the actual searching when looking for Used or Fresh blocks and do this
  //kind of return when looking for unused or spoiled blocks, as the search algorithm for used or fresh blocks
  //can just skip over the dummy values in the block arrays whereas we'd need to put in extra logic to account for
  //said dummy values if we were doing array searching to figure out unused or spoiled blocks instead
  return TotalBlocks - rovePermaMem_getTotalUsedBlocks();
}

uint16_t rovePermaMem_getTotalFreshBlocks()
{
  return getTotalUnusedOrFreshBlocks(getFresh);
}

uint16_t rovePermaMem_getBytesPerBlock()
{
  return BytesPerBlock;
}

uint16_t rovePermaMem_getTotalBlocks()
{
  return TotalBlocks;
}

RovePermaMem_Error rovePermaMem_isBlockUsed(uint16_t blockReference, bool *retVal)
{
  if(blockReference >= TotalBlocks)
  {
    return RovePermaMem_InputOutOfBounds;
  }

  *retVal = isBlockFreshOrUsed(blockReference, getUsed);

  return RovePermaMem_Success;
}

RovePermaMem_Error rovePermaMem_isBlockFresh(uint16_t blockReference, bool *retVal)
{
  if(blockReference >= TotalBlocks)
  {
    return RovePermaMem_InputOutOfBounds;
  }

  *retVal = isBlockFreshOrUsed(blockReference, getFresh);

  return RovePermaMem_Success;
}

static bool isBlockFreshOrUsed(uint16_t blockReference, bool whichToGet)
{
  word_t blockWord;
  word_t blockReferenceBitband;
  word_t *blockTable;

  if(whichToGet == getFresh)
  {
    blockTable = tm4c1294ncpdt_blockFreshTable;
  }
  else
  {
    blockTable = tm4c1294ncpdt_blockUsedTable;
  }

  //global table is expressed in words
  if(blockReference < bitsPerWord)
  {
    blockWord = blockTable[0];
  }
  else if(blockReference >= bitsPerWord * 2)
  {
    blockWord = blockTable[2];
    blockReference -= bitsPerWord * 2;
  }
  else
  {
    blockWord = blockTable[1];
    blockReference -= bitsPerWord;
  }

  blockReferenceBitband = blockCountToBitbandWord(blockReference);

  return blockReferenceBitband & blockWord;
}

static uint16_t getTotalUnusedOrFreshBlocks(bool whichToGet)
{
  int i, j;
  int count = 0;
  word_t bitBand;
  word_t *tableReference;

  if(whichToGet == getFresh)
  {
    tableReference = tm4c1294ncpdt_blockFreshTable;
  }
  else
  {
    tableReference = tm4c1294ncpdt_blockUsedTable;
  }

  //90 total available blocks, but kept in an array of 32 bits. So, search through the array by checking
  // each of the 32 bits in each of its indexes.
  for(i = 0; i < BlockReferenceTableSize; i++)
  {
    for(j = 0; j < bitsPerWord; j++)
    {
      bitBand = blockCountToBitbandWord(j);
      if(bitBand & tableReference[i])
      {
        //technically everything from 90 to 96 are dummy values, but they don't affect the count because rovePermaMem_init sets them to 0
        count++;
      }
    }
  }

  return count;
}

//our tables organize information on the blocks using bool information stored in the 32 bit words, IE
// whether or not the first block is fresh is expressed in the first bit of the fresh table, the second is expressed
//in the second bit, etc.
//This function converts a block count -- 0 to 89 -- and converts it into a word that can be used in binary operation to
//change its expression in the binary tables.
static word_t blockCountToBitbandWord(uint16_t x)
{
  #ifndef ROVEDEBUG_NO_DEBUG
  if(x >= bitsPerWord)
  {
    debugFault("You idiot drue, you didn't constrain your inputs properly");
  }
  #endif

  //our tables go from left to right for counting
  return 0b10000000000000000000000000000000 >> x;
}

static uint16_t inputBlockIndexToMemIndex(uint16_t inputBlockIndex)
{
  return inputBlockIndex += BlockIndex_ReservedBlocks;
}

static void unlockEeprom()
{
  uint32_t writeVal;

  writeVal = ControlBlockPassword;
  EEPROMBlockUnlock(BlockIndex_ControlBlock, &writeVal, 1);
}
static void lockEeprom()
{
  EEPROMBlockLock(BlockIndex_ControlBlock);
}

static void updateUseTable(uint16_t blockReference, bool setUse)
{
  uint8_t tableIndex;
  word_t bitband;
  if(blockReference < bitsPerWord)
  {
    tableIndex = 0;
  }
  else if(blockReference >= bitsPerWord * 2)
  {
    tableIndex = 2;
    blockReference -= bitsPerWord * 2;
  }
  else
  {
    tableIndex = 1;
    blockReference -= bitsPerWord;
  }

  bitband = blockCountToBitbandWord(blockReference);

  if(!setUse)
  {
    tm4c1294ncpdt_blockUsedTable[tableIndex] &= ~bitband;
  }
  else
  {
    tm4c1294ncpdt_blockUsedTable[tableIndex] |= bitband;
  }
}

//precall: must have eeprom unlocked
static void updateFreshTable(uint16_t blockReference)
{
  //if block already noted as being spoilt, don't rewrite it into the eeprom as eeprom has limited amounts of writes allowed before it dies
  bool b;
  rovePermaMem_isBlockFresh(blockReference, &b);
  if(!b)
  {
    return;
  }

  uint8_t tableIndex;
  word_t bitband;
  if(blockReference < bitsPerWord)
  {
    tableIndex = 0;
  }
  else if(blockReference >= bitsPerWord * 2)
  {
    tableIndex = 2;
    blockReference -= bitsPerWord * 2;
  }
  else
  {
    tableIndex = 1;
    blockReference -= bitsPerWord;
  }

  bitband = blockCountToBitbandWord(blockReference);
  tm4c1294ncpdt_blockFreshTable[tableIndex] &= ~bitband;
  EEPROMProgram(tm4c1294ncpdt_blockFreshTable, EEPROMAddrFromBlock(BlockIndex_ControlBlock) + ControlBlock_WordOffset_blockFresh, ControlBlock_WordLength_blockFresh);
}

void rovePermaMem_Init()
{
  uint32_t readValue;

  //try turning on the eeprom
  SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
  if(EEPROMInit() == EEPROM_INIT_ERROR)
  {
    SysCtlDelay(1000);
    if(EEPROMInit() == EEPROM_INIT_ERROR)
    {
      debugFault("EEPROM has suffered from fatal internal error. This could mean its lifespan has been exceeded");
    }
  }

  //check if this is the first the eeprom has ever been used. If so, initialize it with our startup settings.
  //Either way, by default EEPROM starts off letting us read and write to blocks that aren't password protected (and none are password
  //protected on reset) so we can just get straight to business
  EEPROMRead(&readValue, BlockAddress_ControlBlock + ControlBlock_WordOffset_initFootprint, ControlBlock_WordLength_initFootprint);
  if(readValue != InitFootprintKeyword)
  {
    doFirstInit();
  }

  //make sure to set up the tables the non private files use before leaving
  setupGlobalTables();

  //set up protection so that nothing can access the EEPROM's blocks unless a password has been set for said blocks and is used.
  //Used to make the whole thing thread safe even with interrupts, as even if interrupted in the middle of a RovePermaMem function call
  //other parts of the program still wont' be able to mess with the EEPROM.
  EEPROMBlockProtectSet(BlockAddress_ControlBlock, EEPROM_PROT_NA_LNA_URW);

  //lock all blocks in the system by writing a password to block 0 and locking it, which locks the whole system until we unlock block 0 again
  uint32_t writeVal = ControlBlockPassword;
  EEPROMBlockPasswordSet(BlockAddress_ControlBlock, &writeVal, 1);
  EEPROMBlockLock(BlockAddress_ControlBlock);
}

static void doFirstInit()
{
  uint32_t writeValArray[3];

  //make sure that blockInformation word is set to our default state.
  writeValArray[0] = 0xFFFFFFFF;
  writeValArray[1] = 0xFFFFFFFF;
  writeValArray[2] = 0xFFFFFFC0; //only set up to the 89th block, rest are dummy values
  EEPROMProgram(writeValArray, BlockAddress_ControlBlock + ControlBlock_WordOffset_blockFresh, ControlBlock_WordLength_blockFresh);

  //set our footprint into the block we've allocated for it, so that we'll know on next startup that this operation was performed.
  writeValArray[0] = InitFootprintKeyword;
  EEPROMProgram(writeValArray, BlockAddress_ControlBlock + ControlBlock_WordOffset_initFootprint, ControlBlock_WordLength_initFootprint);
}

static void setupGlobalTables()
{
  int i;

  //the table that tracks what blocks are being used starts off completely empty, as no blocks have yet to
  //be declared in use on startup
  for(i = 0; i < BlockReferenceTableSize; i++)
  {
    tm4c1294ncpdt_blockUsedTable[i] = 0;
  }

  //block fresh table, meanwhile, comes from control block's word registers 1, 2, and 3
  EEPROMRead(tm4c1294ncpdt_blockFreshTable, BlockAddress_ControlBlock + ControlBlock_WordOffset_blockFresh, ControlBlock_WordLength_blockFresh);

  //should have already erased the dummy values in the eeprom's first initialization, but let's sanity check it everytime we turn it
  //on after. DummyMask sets them all to 0
  tm4c1294ncpdt_blockFreshTable[2] &= DummyMaskInLastWordOfTable;
}
