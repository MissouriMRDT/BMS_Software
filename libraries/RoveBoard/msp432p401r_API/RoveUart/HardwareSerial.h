/*
 * HardwareSerial.h
 *
 *  Created on: Oct 16, 2017
 *      Author: drue
 */

#ifndef ROVEBOARD_MSP432P401R_API_ROVEUART_HARDWARESERIAL_H_
#define ROVEBOARD_MSP432P401R_API_ROVEUART_HARDWARESERIAL_H_

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include <inttypes.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../supportingUtilities/new.h"
#include "../msp432Ware/driverlib/driverlib.h"

#define SERIAL_BUFFER_SIZE     17

//constants to set stop bits in transmission
#define OneStopBit 0
#define TwoStopBit 1

//constants to set up parity bits in transmission
#define NoParity 0
#define EvenParity 1
#define OddParity 2

class HardwareSerial
{
  private:
    bool transmitting;
    unsigned char *txBuffer;
    unsigned long txBufferSize;
    unsigned long txWriteIndex;
    unsigned long txReadIndex;
    unsigned char *rxBuffer;
    unsigned long rxBufferSize;
    unsigned long rxWriteIndex;
    unsigned long rxReadIndex;
    unsigned long uartModule;
    eUSCI_UART_Config config;
    void primeTransmit();
    void configureForBaud(uint32_t baud);
    uint8_t lookupSecondModReg(float N);

  public:
    HardwareSerial(void);
    HardwareSerial(unsigned long);
    void begin(unsigned long, uint8_t, uint8_t);
    void end(void);
    void setBufferSize(unsigned long);
    unsigned long getBufferSize();
    int available(void);
    int peek(void);
    int peek(uint16_t index);
    int read(void);
    void flush(void);
    void flushAll(void);
    void UARTIntHandler(void);
    size_t write(uint8_t c);
    size_t write(const uint8_t *buffer, size_t size);
    size_t println(const char* str);
    operator bool();
    void setOutputSettings(uint8_t paritySettings, uint8_t stopBitSettings);
};

extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial2;
extern HardwareSerial Serial3;

void attachTransmitCb(void (*userFunc)(uint8_t));
void attachReceiveCb(void (*userFunc)(uint8_t));

extern "C" void UARTIntHandler0(void);
extern "C" void UARTIntHandler1(void);
extern "C" void UARTIntHandler2(void);
extern "C" void UARTIntHandler3(void);

#endif




#endif /* ROVEBOARD_MSP432P401R_API_ROVEUART_HARDWARESERIAL_H_ */
