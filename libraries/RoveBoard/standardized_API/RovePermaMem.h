/*
 * RovePermaMem.h
 *
 *  Created on: Oct 20, 2017
 *      Author: drue satterfield (drscp5@smt.edu)
 *
 *
 *
 *  API for accessing the chip's permanent memory. Use this to store information you don't want to be erased on reset or power loss.
 *  Note that this has a limited life span; after some amount of writes, a single block will no longer be usable. How many is too many
 *  is chip-dependant, check the chip specific H files for more info.
 *
 *  This library enforces protection of the permanent memory by making it so that processes have to explicitly allocate blocks of memory for usage
 *  using the useBlock function, and will keep processes from using blocks of memory already allocated.
 *
 *  More information such as whether or not the system is natively thread safe is noted in the board specific H files.
 *      Thread safe on:
 *          tiva tm4c1294ncpdt
 */

#ifndef ROVEBOARD_STANDARDIZED_API_ROVEPERMAMEM_H_
#define ROVEBOARD_STANDARDIZED_API_ROVEPERMAMEM_H_

#include <stdint.h>

typedef enum
{
  RovePermaMem_Success,
  RovePermaMem_InputOutOfBounds,
  RovePermaMem_ImproperPassword,
  RovePermaMem_AlreadyUsed,
  RovePermaMem_BlockNotAllocated
} RovePermaMem_Error;

typedef struct
{
  uint16_t blockReference;
  uint16_t password;
} RovePermaMem_Block;

//overview: allocates a block in permanent memory for usage. Must be called before any read or writes can be done.
//Inputs:   blockReference: a number saying which of the blocks you want to use. Index 0, so proper inputs are 0 onward. Limit is board specific.
//          passwordToUse:  a password for the system to internally use for this block.
//
//returns:  a reference to a now initialized block
//Warning:  You can't call this twice for the same block, it's a one and done until reset. If you lose the block reference struct
//          or somehow lose the password within it, well, there ain't no sending a backup to your gmail to recover it.
extern RovePermaMem_Block rovePermaMem_useBlock(uint16_t blockReference, uint16_t passwordToUse);

//overview: writes a byte into your permanent memory block.
//Inputs:   blockHandle:    your block's handle, returned from useBlock
//          byteReference:  selects which byte in the block you want to write to. From 0 onward, up to the amount of bytes in each block.
//          valueToWrite:   The value of the byte to write into permanent memory
//
//returns:  error information. Success if nothing went wrong, InputOutOfBounds if byteReference isn't properly constrained (or if the struct's
//          internal block reference is wrong, which would mean you set it up wrong in useBlock), blockNotAllocated if you haven't called
//          useBlock, or ImproperPassword if the struct's password was wrong, which probably means you're somehow trying to use someone else's
//          block.
extern RovePermaMem_Error rovePermaMem_WriteBlockByte(RovePermaMem_Block blockHandle, uint8_t byteReference, uint8_t valueToWrite);

//overview: writes values into your entire memory block
//Inputs:   blockHandle: your block's handle, returned from useBlock
//          bytes:       selects which byte in the block you want to write to. Should be the size of the amount of bytes per block, unless
//                       you're fine with the function filling undefined values into permanent memory for the rest of the size.
//
//returns:  error information. Success if nothing went wrong, InputOutOfBounds if bytes[] is null (or if the struct's
//          internal block reference is wrong, which would mean you set it up wrong in useBlock), blockNotAllocated if you haven't called
//          useBlock, or ImproperPassword if the struct's password was wrong, which probably means you're somehow trying to use someone else's
//          block.
extern RovePermaMem_Error rovePermaMem_WriteBlock(RovePermaMem_Block blockHandle, uint8_t bytes[]);

//overview: reads a byte from your permanent memory block.
//Inputs:   blockHandle:    your block's handle, returned from useBlock
//          byteReference:  selects which byte in the block you want to read from. From 0 onward, up to the amount of bytes per block
//          readBuffer:     The pass-by-pointer return value. Will contain the read byte upon return
//
//returns:  error information. Success if nothing went wrong, InputOutOfBounds if byteReference isn't properly constrained (or if the struct's
//          internal block reference is wrong, which would mean you set it up wrong in useBlock), blockNotAllocated if you haven't called
//          useBlock, or ImproperPassword if the struct's password was wrong, which probably means you're somehow trying to use someone else's
//          block.
extern RovePermaMem_Error rovePermaMem_ReadBlockByte(RovePermaMem_Block blockHandle, uint8_t byteReference, uint8_t *readBuffer);

//overview: reads all the values from your entire memory block
//Inputs:   blockHandle: your block's handle, returned from useBlock
//          bytes:       pass-by-pointer return value. Will contain all of the read bytes upon return. MUST contain at least the amount of bytes
//                       per block, which is how many bytes the function fills it with, unless you want memory leaks to ensue (you don't).
//
//returns:  error information. Success if nothing went wrong, InputOutOfBounds if byteBuffer is null (or if the struct's
//          internal block reference is wrong, which would mean you set it up wrong in useBlock), blockNotAllocated if you haven't called
//          useBlock, or ImproperPassword if the struct's password was wrong, which probably means you're somehow trying to use someone else's
//          block.
extern RovePermaMem_Error rovePermaMem_ReadBlock(RovePermaMem_Block blockHandle, uint8_t byteBuffer[]);

//overview: Gets the first block that's available for using in the system.
//Inputs:   onlyGetFreshBlocks: if true, it will only get blocks that have never ever been used. If false, will just look for blocks not currently
//                              in use.
//          startingBlock:      Which block to start the search from, going up. 0 means it'll look at 0 then go to 1, 2, etc, 1 means it'll look
//                              at 1 then go to 2, 3, 4, etc.
//          ret_blockReference: pass-by-pointer return value. Will contain the reference to the found block, from 0 to the amount of blocks
//                              available on this system, if any are found at all.
//
//returns:  true if a block was found, false if no block was found
extern bool rovePermaMem_getFirstAvailableBlock(bool onlyGetFreshBlocks, uint16_t startingBlock, uint16_t* ret_blockReference);

//overview: Gets the total amount of blocks that have been declared for use in this program so far.
//returns:  number of blocks currently in use, 0 if none, goes up to the amount of blocks in the system
//note:     bear in mind, this return value isn't 0 index so can't be directly used by most functions.
extern uint16_t rovePermaMem_getTotalUsedBlocks();

//overview: Gets the total amount of blocks that have not been declared for use in this program so far.
//returns:  number of blocks not currently in use, 0 if all are used, goes up to the amount of blocks in the system
//note:     bear in mind, this return value isn't 0 index so can't be directly used by most functions.
extern uint16_t rovePermaMem_getTotalUnusedBlocks();

//overview: Gets the total amount of blocks that have never been used on this chip.
//returns:  number of blocks that have never been used, 0 if all have been used, goes up to the amount of blocks in the system
//note:     bear in mind, this return value isn't 0 index so can't be directly used by most functions.
extern uint16_t rovePermaMem_getTotalFreshBlocks();

//overview: Gets the total amount of bytes contained in each block of memory on this chip.
//note:     bear in mind, this return value isn't 0 index. Most byte references in other functions are constrained from 0 index instead. So this
//          value minus one.
extern uint16_t rovePermaMem_getBytesPerBlock();

//overview: Gets the total amount of blocks available on this chip.
//note:     bear in mind, this return value isn't 0 index. Most byte references in other functions are constrained from 0 index instead. So this
//          value minus one.
extern uint16_t rovePermaMem_getTotalBlocks();

//overview: Gets whether or not a block is currently in use.
//Inputs:   blockReference: a reference to which block to check, 0 to 89.
//          retVal:         a pass-by-pointer return value. Contains whether or not the block in question is being used on return. True if yes.
//returns:  Success if blockReference was between 0 to 89, InputOutOfBounds if not
extern RovePermaMem_Error rovePermaMem_isBlockUsed(uint16_t blockReference, bool *retVal);

//overview: Gets whether or not a block on this chip has ever been used.
//Inputs:   blockReference: a reference to which block to check, 0 to 89.
//          retVal:         a pass-by-pointer return value. Contains whether or not the block in question has been used on return. True if yes.
//returns:  Success if blockReference was between 0 to 89, InputOutOfBounds if not
extern RovePermaMem_Error rovePermaMem_isBlockFresh(uint16_t blockReference, bool *retVal);


#endif /* ROVEBOARD_STANDARDIZED_API_ROVEPERMAMEM_H_ */
