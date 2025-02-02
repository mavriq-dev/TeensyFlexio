/* Based on the wonderful work of https://github.com/KurtE Many Many Thanks!
 * The repository is at https://github.com/KurtE/FlexIO_t4
 */



#pragma once

#include "Flexio_t4.h"
#include "TeensyFlexIO.h"

class TeensyFlexSerial : public Stream, public FlexIOHandlerCallback {
private:
    TeensyFlexIO _tx_flexio;
    TeensyFlexIO _rx_lexio;
    int8_t _tx_pin;
    int8_t _rx_pin;
    int8_t _tx_flex_number;
    int8_t _rx_flex_number;
    int8_t _tx_shifter;
    int8_t _tx_timer;
    int8_t _rx_shifter;
    int8_t _rx_timer;


    static const uint16_t TX_BUFFER_SIZE = 64;
    uint8_t _tx_buffer[TX_BUFFER_SIZE];
    volatile uint16_t _tx_buffer_head = 0;
    volatile uint16_t _tx_buffer_tail = 0;
    volatile uint8_t _transmitting = 0;
    static const uint16_t RX_BUFFER_SIZE = 40;
    uint8_t _rx_buffer[RX_BUFFER_SIZE];
    volatile uint16_t _rx_buffer_head = 0;
    volatile uint16_t _rx_buffer_tail = 0;
    static const uint32_t FLUSH_TIMEOUT = 1000;	

    void printDebugInfo();

public:
    TeensyFlexSerial(int8_t txPin = -1, int8_t rxPin = -1,
        int8_t txFlexIO = -1, 
        int8_t txShifter = -1, int8_t txTimer = -1, 
        int8_t rxFlexIO = -1, 
        int8_t rxShifter = -1, int8_t rxTimer = -1) :
        _tx_pin(txPin), _rx_pin(rxPin), 
        _tx_flex_number(txFlexIO), _rx_flex_number(rxFlexIO), 
        _tx_shifter(txShifter), _tx_timer(txTimer), 
        _rx_shifter(rxShifter), _rx_timer(rxTimer) {};
    ~TeensyFlexSerial();

    void begin(uint32_t baud = 115200, uint16_t format = 0);
    void end(void){};
    int availableForWrite(void);
    void clear(void);
    int available(void);
    int peek(void);
    int read(void);
    void flush(void);
    using Print::write;

    float setClock(float frequency);
	float setClockUsingAudioPLL(float frequency);
	float setClockUsingVideoPLL(float frequency);
    
    virtual bool call_back(FlexIOHandler *pflex)  ;

    // Move our writeChar method here
    size_t write(uint8_t c);
};