/* Based on the wonderful work of https://github.com/KurtE Many Many Thanks!
 * The repository is at https://github.com/KurtE/FlexIO_t4
 */

#include "TeensyFlexIO.h"
#include <Arduino.h>
#include <DMAChannel.h>
#include <EventResponder.h>

#ifndef _TEENSY_FLEXIO_SPI_H_
#define _TEENSY_FLEXIO_SPI_H_

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#ifndef DEFAULT_TRANSFER_BITS
#define DEFAULT_TRANSFER_BITS 8
#define DEFAULT_TRANSFER_BYTES 1
#endif

#ifndef SPI_MODE0
#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C
#endif
#define SPI_MODE_TRANSMIT_ONLY 0x80 // Hack to allow higher speeds when transmit only

// Pretty stupid settings for now...
class TeensyFlexSPISettings {
  public:
    TeensyFlexSPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) : _clock(clock),
                                                                            _bitOrder(bitOrder), _dataMode(dataMode), _nTransferBits(DEFAULT_TRANSFER_BITS){};

    TeensyFlexSPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode, uint8_t nTransBits) : _clock(clock),
                                                                                                _bitOrder(bitOrder), _dataMode(dataMode), _nTransferBits(nTransBits){};

    uint32_t _clock;
    uint8_t _bitOrder;
    uint8_t _dataMode;
    uint8_t _nTransferBits;
};

class TeensyFlexSPI : public FlexIOHandlerCallback {
  public:
    enum { TX_BUFFER_SIZE = 64,
           RX_BUFFER_SIZE = 40 };
    TeensyFlexSPI(int mosiPin, int misoPin, int sckPin, int csPin = -1) : _mosiPin(mosiPin), _sckPin(sckPin), _misoPin(misoPin), _csPin(csPin){};

    ~TeensyFlexSPI() { end(); }
    bool begin(int flexio_module);
    void end(void);

    uint8_t transfer(uint8_t b) { return (uint8_t)transferNBits((uint32_t)b, sizeof(b) * 8); }      // transfer 1 byte
    uint16_t transfer16(uint16_t w) { return (uint16_t)transferNBits((uint32_t)w, sizeof(w) * 8); } // transfer 2 bytes
    uint32_t transfer32(uint32_t w) { return (uint32_t)transferNBits(w, sizeof(w) * 8); }           // transfer 4 bytes
    uint32_t transferNBits(uint32_t w_out, uint8_t nbits);                                          // transfer arbitrary number of bits up to 32

    void setShiftBufferOut(uint32_t val, uint8_t nbits);
    void setShiftBufferOut(const void *buf, uint8_t nbits, size_t dtype_size);
    uint32_t getShiftBufferIn(uint8_t nbits);
    void getShiftBufferIn(void *retbuf, uint8_t nbits, size_t dtype_size);

    void inline transfer(void *buf, size_t count) { transfer(buf, buf, count); }
    void setTransferWriteFill(uint8_t ch) { _transferWriteFill = ch; }
    void transfer(const void *buf, void *retbuf, size_t count) { transferBufferNBits(buf, retbuf, count, 0); } // 0 on nbits implies use object state
    void transferBufferNBits(const void *buf, void *retbuf, size_t count, uint8_t nbits);

    bool transfer(const void *txBuffer, void *rxBuffer, size_t count, EventResponderRef event_responder);

    static void _dma_rxISR0(void);
    static void _dma_rxISR1(void);
    inline void dma_rxisr(void);

    void beginTransaction(TeensyFlexSPISettings settings);
    void endTransaction(void);

    FlexIOHandler *flexIOHandler() { return _flexIO->getFlexIOHandler(); }

    // Call back from flexIO when ISR hapens
    virtual bool call_back(FlexIOHandler *pflex);

  private:
    int _mosiPin;
    int _sckPin;
    int _misoPin;
    int _csPin;

    TeensyFlexIO* _flexIO;

    uint8_t _transferWriteFill = 0;
    uint8_t _in_transaction_flag = 0;

    uint32_t _clock = 0;
    uint8_t _bitOrder = MSBFIRST;
    uint8_t _dataMode = SPI_MODE0;
    uint8_t _nTransferBits = DEFAULT_TRANSFER_BITS;
    uint8_t _nTransferBytes = DEFAULT_TRANSFER_BYTES; // Calculated during beginTransaction from _nTransferBits, used for DMA transfers
    volatile uint32_t *_shiftBufInReg = nullptr;
    volatile uint32_t *_shiftBufOutReg = nullptr;

    uint8_t _timer = 0xff;
    uint8_t _tx_shifter = 0xff;
    uint8_t _rx_shifter = 0xff;

    // DMA - Async support
    bool initDMAChannels();
    enum DMAState { notAllocated,
                    idle,
                    active,
                    completed };
    enum { MAX_DMA_COUNT = 32767 };
    DMAState _dma_state = DMAState::notAllocated;
    uint32_t _dma_count_remaining = 0; // How many bytes left to output after current DMA completes
    DMAChannel *_dmaTX = nullptr;
    DMAChannel *_dmaRX = nullptr;
    EventResponder *_dma_event_responder = nullptr;
    static TeensyFlexSPI *_dmaActiveObjects[FlexIOHandler::CNT_FLEX_IO_OBJECT];
};
#endif //_TEENSY_FLEX_SPI_H_