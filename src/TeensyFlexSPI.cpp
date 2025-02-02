/* Based on the wonderful work of https://github.com/KurtE Many Many Thanks!
 * The repository is at https://github.com/KurtE/FlexIO_t4
 */

#include "TeensyFlexSPI.h"
#define BAUDRATE 115200
#define FLEXIO1_CLOCK (480000000L / 16) // Again assuming default clocks?


TeensyFlexSPI *TeensyFlexSPI::_dmaActiveObjects[FlexIOHandler::CNT_FLEX_IO_OBJECT] = {nullptr, nullptr};

//=============================================================================
// TeensyFlexSPI::Begin
//=============================================================================
bool TeensyFlexSPI::begin(int flexio_module) {
    // BUGBUG - may need to actual Clocks to computer baud...
    //	uint16_t baud_div =  (FLEXIO1_CLOCK/baud)/2 - 1;
    //-------------------------------------------------------------------------
    // Make sure all of the IO pins are valid flex pins on same controller
    //-------------------------------------------------------------------------

     _flexIO = new TeensyFlexIO();
     _flexIO->begin(static_cast<TeensyFlexIO::FlexIOModule>(flexio_module) );
     Serial.println("FlexIO1 begin");
     Serial.flush();

    // Now reserve timers and shifters
    _timer = _flexIO->requestTimers((_csPin != -1) ? 2 : 1);
    _tx_shifter = _flexIO->requestShifter();
    _rx_shifter = _flexIO->requestShifter(_flexIO->shiftersDMAChannel(_tx_shifter));
    Serial.println("FlexIO1 shifters");
    Serial.flush();

    // If first request failed to get second different shifter on different dma channel, allocate other one on same channel
    // but DMA will not work...
    if (_rx_shifter == 0xff)
        _rx_shifter = _flexIO->requestShifter();

    if ((_timer == 0xff) || (_tx_shifter == 0xff) || (_rx_shifter == 0xff)) {
        _flexIO->getFlexIOHandler()->freeTimers(_timer, (_csPin != -1) ? 2 : 1);
        _timer = 0xff;
        _flexIO->getFlexIOHandler()->freeShifter(_tx_shifter);
        _tx_shifter = 0xff;
        _flexIO->getFlexIOHandler()->freeShifter(_rx_shifter);
        _rx_shifter = 0xff;
        #ifdef DEBUG_FlexSPI
                Serial.println("TeensyFlexSPI - Failed to allocate timers or shifters");
        #endif
        return false;
    }

    #ifdef DEBUG_FlexSPI
        DEBUG_FlexSPI.printf("timer index: %d shifter index: %d mask: %x\n", _timer, _tx_shifter, SHIFTER_MASK(_tx_shifter));
        // lets try to configure a tranmitter like example
        DEBUG_FlexSPI.println("Before configure flexio");
    #endif

    ShifterConfig tx_shifter_config;
    tx_shifter_config.mode = ShifterMode::Transmit;
    tx_shifter_config.timerPolarity = TimerPolarity::ActiveLow;
    tx_shifter_config.pinConfig = PinConfig::Output;
    tx_shifter_config.timerSelect = _timer;
    tx_shifter_config.pinSelect = _mosiPin;
    _flexIO->configureShifter(_tx_shifter, tx_shifter_config);

    ShifterConfig rx_shifter_config;
    rx_shifter_config.mode = ShifterMode::Receive;
    rx_shifter_config.timerSelect = _timer;
    rx_shifter_config.pinSelect = _misoPin;
    _flexIO->configureShifter(_rx_shifter, rx_shifter_config);


    if (_csPin != -1) {
        TimerConfig timer_config;
        timer_config.mode = TimerMode::Baud;
        timer_config.pinSelect = _sckPin;
        timer_config.pinConfig = PinConfig::Disabled;
        timer_config.timerOutput = TimerOutput::Zero;
        timer_config.timerDisable = TimerDisable::OnCompare;
        timer_config.timerEnable = TimerEnable::TriggerHigh;
        timer_config.timerReset = TimerReset::Never;
        timer_config.timerDecrement = TimerDecrement::FlexIOClock;
        timer_config.triggerSelect = _flexIO->calculateTriggerSelect(TriggerType::SHIFTER, 0);
        timer_config.triggerPolarity = TriggerPolarity::ActiveLow;
        timer_config.triggerSource = TriggerSource::Internal;
        timer_config.stopBit = 2;
        timer_config.startBit = 1;
        timer_config.asDual().bits_in_word = 0xF;
        timer_config.asDual().baud_rate_div = 0x1;
        _flexIO->configureTimer(_timer, timer_config);
        Serial.println("incs pin");

        TimerConfig timer2_config;
        timer2_config.mode = TimerMode::SingleCounter;
        timer2_config.pinSelect = _csPin;
        timer2_config.pinConfig = PinConfig::Output;
        timer2_config.pinPolarity = PinPolarity::ActiveLow;
        timer2_config.timerOutput = TimerOutput::One;
        timer2_config.timerDisable = TimerDisable::N1Disable;
        timer2_config.timerEnable = TimerEnable::N1Enable;
        timer2_config.timerReset = TimerReset::Never;
        timer2_config.timerDecrement = TimerDecrement::FlexIOClock;
        timer2_config.triggerSelect = 0;
        timer2_config.triggerPolarity = TriggerPolarity::ActiveHigh;
        timer2_config.triggerSource = TriggerSource::External;
        timer2_config.stopBit = 0;
        timer2_config.startBit = 0;
        timer2_config.asDual().bits_in_word = 0xFF;
        timer2_config.asDual().baud_rate_div = 0xFF;
        _flexIO->configureTimer(_timer + 1, timer2_config);

    } else {
        Serial.println("not in cs pin");
        TimerConfig timer_config;
        timer_config.mode = TimerMode::Baud;
        timer_config.pinSelect = _sckPin;
        timer_config.pinConfig = PinConfig::Disabled;
        timer_config.timerOutput = TimerOutput::Zero;
        timer_config.timerDisable = TimerDisable::OnCompare;
        timer_config.timerEnable = TimerEnable::TriggerHigh;
        timer_config.timerReset = TimerReset::Never;
        timer_config.timerDecrement = TimerDecrement::FlexIOClock;
        timer_config.triggerSelect = _flexIO->calculateTriggerSelect(TriggerType::SHIFTER, 0);
        timer_config.triggerPolarity = TriggerPolarity::ActiveLow;
        timer_config.triggerSource = TriggerSource::Internal;
        timer_config.stopBit = 0;
        timer_config.startBit = 0;
        timer_config.asDual().bits_in_word = 0xF;
        timer_config.asDual().baud_rate_div = 0x1;
        _flexIO->configureTimer(_timer, timer_config);
    }

    Serial.println("FlexIO1 config");
    Serial.flush();

    // Make sure this flex IO object is enabled
    _flexIO->enable();

    // Set the IO pins into FLEXIO mode
    _flexIO->setPinFlexioMode(_mosiPin);
    _flexIO->setPinFlexioMode(_sckPin);
    _flexIO->setPinFlexioMode(_misoPin);

    // Cofigure the port config registers like SPI does
    _flexIO->setPinParameters(_mosiPin, PullUp::DISABLED, 7, 3);
    _flexIO->setPinParameters(_sckPin, PullUp::DISABLED, 7, 3);
    _flexIO->setPinParameters(_misoPin, PullUp::PULLUP_22K, 7, 3);

    if (_csPin != -1)
        _flexIO->setPinFlexioMode(_csPin);

    _flexIO->getFlexIOHandler()->addIOHandlerCallback(this);

    // Set up pointers to the bit-swapped shift registers for MSB first transfer
    _bitOrder = MSBFIRST;
    _shiftBufOutReg = &_flexIO->getFlexIOHandler()->port().SHIFTBUFBBS[_tx_shifter];
    _shiftBufInReg = &_flexIO->getFlexIOHandler()->port().SHIFTBUFBIS[_rx_shifter];

    Serial.println("FlexIO1 config done");

    // Lets print out some of the settings and the like to get idea of state
    #ifdef DEBUG_FlexSPI
        IMXRT_FLEXIO_t *p = _flexIO->getFlexIO();
        uint8_t _sck_flex_pin  = _flexIO->getFlexIOHandler()->mapIOPinToFlexPin(_sckPin);
        uint8_t _miso_flex_pin = _flexIO->getFlexIOHandler()->mapIOPinToFlexPin(_misoPin);
        uint8_t _mosi_flex_pin = _flexIO->getFlexIOHandler()->mapIOPinToFlexPin(_mosiPin);

        DEBUG_FlexSPI.printf("Mosi map: %d %x %d\n", _mosiPin, (uint32_t)_flexIO->getFlexIOHandler(), _mosi_flex_pin);
        DEBUG_FlexSPI.printf("Miso map: %d %d\n", _misoPin, _miso_flex_pin);
        DEBUG_FlexSPI.printf("Sck map: %d %d\n", _sckPin, _sck_flex_pin);
        DEBUG_FlexSPI.printf("CCM_CDCDR: %x\n", CCM_CDCDR);
        DEBUG_FlexSPI.printf("FlexIO bus speed: %d\n", _flexIO->getFlexIOHandler()->computeClockRate());
        DEBUG_FlexSPI.printf("VERID:%x PARAM:%x CTRL:%x PIN: %x\n", p->PARAM, p->CTRL, p->CTRL, p->PIN);
        DEBUG_FlexSPI.printf("SHIFTSTAT:%x SHIFTERR=%x TIMSTAT=%x\n", p->SHIFTSTAT, p->SHIFTERR, p->TIMSTAT);
        DEBUG_FlexSPI.printf("SHIFTSIEN:%x SHIFTEIEN=%x TIMIEN=%x\n", p->SHIFTSIEN, p->SHIFTEIEN, p->TIMIEN);
        DEBUG_FlexSPI.printf("SHIFTSDEN:%x SHIFTSTATE=%x\n", p->SHIFTSDEN, p->SHIFTSTATE);
        DEBUG_FlexSPI.printf("SHIFTCTL:%x %x %x %x\n", p->SHIFTCTL[0], p->SHIFTCTL[1], p->SHIFTCTL[2], p->SHIFTCTL[3]);
        DEBUG_FlexSPI.printf("SHIFTCFG:%x %x %x %x\n", p->SHIFTCFG[0], p->SHIFTCFG[1], p->SHIFTCFG[2], p->SHIFTCFG[3]);
        DEBUG_FlexSPI.printf("TIMCTL:%x %x %x %x\n", p->TIMCTL[0], p->TIMCTL[1], p->TIMCTL[2], p->TIMCTL[3]);
        DEBUG_FlexSPI.printf("TIMCFG:%x %x %x %x\n", p->TIMCFG[0], p->TIMCFG[1], p->TIMCFG[2], p->TIMCFG[3]);
        DEBUG_FlexSPI.printf("TIMCMP:%x %x %x %x\n", p->TIMCMP[0], p->TIMCMP[1], p->TIMCMP[2], p->TIMCMP[3]);
    #endif

    return true;
}

void TeensyFlexSPI::end(void) {
    // If the transmit was allocated free it now as well as timers and shifters.
    if (_flexIO->getFlexIOHandler()) {
        _flexIO->getFlexIOHandler()->freeTimers(_timer, (_csPin != -1) ? 2 : 1);
        _timer = 0xff;
        _flexIO->getFlexIOHandler()->freeShifter(_tx_shifter);
        _flexIO->getFlexIOHandler()->freeShifter(_rx_shifter);
        _tx_shifter = 0xff;

        _flexIO->getFlexIOHandler()->removeIOHandlerCallback(this);
    }
}

void TeensyFlexSPI::beginTransaction(TeensyFlexSPISettings settings) {

    // right now pretty stupid
    if ((settings._clock != _clock) || (settings._dataMode != _dataMode) || (settings._nTransferBits != _nTransferBits)) {
        _clock = settings._clock;
        _dataMode = settings._dataMode;
        _nTransferBits = settings._nTransferBits; // Probaby should have some safety checking to keep this in the 1-32 range for now.
        _nTransferBytes = (_nTransferBits - 1) / 8 + 1;
        if (_nTransferBytes == 3)
            _nTransferBytes = 4;                               // DMA doesn't handle arbitrary pointer shifts so force 32bit alignment even though it would fit into 24bits.
        uint32_t clock_speed = _flexIO->getFlexIOHandler()->computeClockRate() / 2;
        uint32_t div = clock_speed / _clock;
        if (div) {
            if ((clock_speed / div) > _clock)
                div++; // unless even multiple increment
            div--;     // the actual value stored is the -1...
        }
        if (!(_dataMode & SPI_MODE_TRANSMIT_ONLY)) {
            if (div == 0)
                div = 1; // force to at least one as Reads will fail at 0...
            else if ((div == 1) && (_clock > 30000000u))
                div = 2;
        }

        _flexIO->getFlexIO()->TIMCMP[_timer] = div | (_nTransferBits * 2 - 1) << 8; // Set the clk div for shifter and set transfer length

        #ifdef DEBUG_FlexSPI
                DEBUG_FlexSPI.printf("TeensyFlexSPI:beginTransaction TIMCMP: %x\n", _flexIO->getFlexIO()->TIMCMP[_timer]);
        #endif
    }

    _bitOrder = settings._bitOrder;
}

// After performing a group of transfers and releasing the chip select
// signal, this function allows others to access the SPI bus
void TeensyFlexSPI::endTransaction(void) {
#ifdef DEBUG_FlexSPI
    DEBUG_FlexSPI.printf("TeensyFlexSPI:endTransaction\n");
#endif
}

void TeensyFlexSPI::setShiftBufferOut(uint32_t val, uint8_t nbits) {
    if (_bitOrder == MSBFIRST) {
        _flexIO->getFlexIO()->SHIFTBUFBIS[_tx_shifter] = val << (32 - nbits);
    } else {
        _flexIO->getFlexIO()->SHIFTBUF[_tx_shifter] = val;
    }
}

void TeensyFlexSPI::setShiftBufferOut(const void *buf, uint8_t nbits, size_t dtype_size) {
    uint32_t val = 0;
    switch (dtype_size) {
    case 1:
        val = *(uint8_t *)buf;
        break;
    case 2:
        val = *(uint16_t *)buf;
        break;
    case 3:
        val = (*(uint32_t *)buf) & 0xffffff;
        break;
    case 4:
        val = *(uint32_t *)buf;
        break;
    default:
        break;
    }
    setShiftBufferOut(val, nbits);
}

uint32_t TeensyFlexSPI::getShiftBufferIn(uint8_t nbits) {
    uint32_t ret_val;
    if (_bitOrder == MSBFIRST) {
        ret_val = _flexIO->getFlexIO()->SHIFTBUFBIS[_rx_shifter];
    } else {
        ret_val = _flexIO->getFlexIO()->SHIFTBUF[_rx_shifter] >> (32 - nbits);
    }
    return ret_val;
}

void TeensyFlexSPI::getShiftBufferIn(void *retbuf, uint8_t nbits, size_t dtype_size) {
    uint32_t tmp;
    switch (dtype_size) {
    case 1:
        *(uint8_t *)retbuf = (uint8_t)getShiftBufferIn(nbits);
        break;
    case 2:
        *(uint16_t *)retbuf = (uint16_t)getShiftBufferIn(nbits);
        break;
    case 3: // The weird case that gets some shuffling to handle the last chunk without overflowing
        tmp = getShiftBufferIn(nbits);
        *(uint16_t *)retbuf = (uint16_t)tmp & 0xffff;           // Lower16 bits
        *((uint8_t *)retbuf + 2) = (uint8_t)(tmp >> 16) & 0xff; // Upper8 bits
        break;
    case 4:
        *(uint32_t *)retbuf = (uint32_t)getShiftBufferIn(nbits);
        break;
    default:
        break;
    }
}

uint32_t TeensyFlexSPI::transferNBits(uint32_t w_out, uint8_t nbits) {
    // Need to do some validation...

    uint32_t return_val;
    uint16_t timcmp_save = _flexIO->getFlexIO()->TIMCMP[_timer];                        // remember value coming in
    _flexIO->getFlexIO()->TIMCMP[_timer] = (timcmp_save & 0xff) | (nbits * 2 - 1) << 8; // Adjust transmission length to nbits
    // Serial.printf("TCMP bits = %x\n",_flexIO->getFlexIO()).TIMCMP[_timer]);
    //  Now lets wait for something to come back.
    uint16_t timeout = 0xffff; // don't completely hang
    // Clear any current pending RX input
    if (_flexIO->getFlexIO()->SHIFTSTAT & SHIFTER_MASK(_rx_shifter)) {
        return_val = getShiftBufferIn(nbits);
    }

    setShiftBufferOut(w_out, nbits);

    return_val = 0xff;
    while (!(_flexIO->getFlexIO()->SHIFTSTAT & SHIFTER_MASK(_rx_shifter)) && (--timeout))
        ;

    if (_flexIO->getFlexIO()->SHIFTSTAT & SHIFTER_MASK(_rx_shifter)) {
        return_val = getShiftBufferIn(nbits);
    }

    _flexIO->getFlexIO()->TIMCMP[_timer] = timcmp_save;
    return return_val;
}

void TeensyFlexSPI::transferBufferNBits(const void *buf, void *retbuf, size_t count, uint8_t nbits) {
    if (!nbits)
        nbits = _nTransferBits;
    uint8_t bytestride = (nbits - 1) / 8 + 1;
    if (bytestride == 3) {
        bytestride = 4;
    }
    uint32_t tx_count = count;
    const uint8_t *tx_buffer = (const uint8_t *)buf;
    uint8_t *rx_buffer = (uint8_t *)retbuf;

    if (count <= 0)
        return; // bail if 0 count passed in.

    // put out the first character.
    _flexIO->getFlexIO()->SHIFTERR = SHIFTER_MASK(_rx_shifter) | SHIFTER_MASK(_tx_shifter); // clear out any previous errors
    while (!(_flexIO->getFlexIO()->SHIFTSTAT & SHIFTER_MASK(_tx_shifter)))
        ; // wait for room for the first character

    if (tx_buffer) {
        setShiftBufferOut(tx_buffer, nbits, bytestride);
        tx_buffer += bytestride;
    }
    tx_count--;
    while (tx_count) {
        // wait for room for the next character
        while (!(_flexIO->getFlexIO()->SHIFTSTAT & SHIFTER_MASK(_tx_shifter)))
            ;

        if (tx_buffer) {
            setShiftBufferOut(tx_buffer, nbits, bytestride);
            tx_buffer += bytestride;
        }
        tx_count--;

        // Wait for data to come back
        while (!(_flexIO->getFlexIO()->SHIFTSTAT & SHIFTER_MASK(_rx_shifter)))
            ;

        if (rx_buffer) {
            getShiftBufferIn(rx_buffer, nbits, bytestride);
            rx_buffer += bytestride;
        }
    }
    // wait for last character to come back...
    while (!(_flexIO->getFlexIO()->SHIFTSTAT & SHIFTER_MASK(_rx_shifter)) && !(_flexIO->getFlexIO()->SHIFTERR & SHIFTER_MASK(_rx_shifter)))
        ;

    if (rx_buffer) {
        getShiftBufferIn(rx_buffer, nbits, bytestride);
        rx_buffer += bytestride;
    }
}

bool TeensyFlexSPI::call_back(FlexIOHandler *pflex) {
    //	DEBUG_digitalWriteFast(4, HIGH);
    return false; // right now always return false...
}

//=============================================================================
// ASYNCH Support
//=============================================================================
//=========================================================================
// Try Transfer using DMA.
//=========================================================================
static uint8_t bit_bucket;
#define dontInterruptAtCompletion(dmac) (dmac)->TCD->CSR &= ~DMA_TCD_CSR_INTMAJOR

//=========================================================================
// Init the DMA channels
//=========================================================================
bool TeensyFlexSPI::initDMAChannels() {
    // Allocate our channels.
    _dmaTX = new DMAChannel();
    if (_dmaTX == nullptr) {
        #if DEBUG_FlexSPI
            Serial.println("Failed to allocate DMA TX channel");
        #endif
        return false;
    }

    _dmaRX = new DMAChannel();
    if (_dmaRX == nullptr) {
        delete _dmaTX; // release it
        _dmaTX = nullptr;
        #if DEBUG_FlexSPI
            Serial.println("Failed to allocate DMA RX channel");
        #endif
        return false;
    }

    int iFlexIndex = _flexIO->getFlexIOHandler()->FlexIOIndex();
    // Let's setup the RX chain
    _dmaRX->disable();
    _dmaRX->source((volatile uint8_t &)*_shiftBufInReg);
    _dmaRX->disableOnCompletion();
    _dmaRX->triggerAtHardwareEvent(_flexIO->getFlexIOHandler()->shiftersDMAChannel(_rx_shifter));
    if (iFlexIndex == 0) {
        _dmaRX->attachInterrupt(&_dma_rxISR0);
        _dmaActiveObjects[0] = this;
    } else {
        _dmaRX->attachInterrupt(&_dma_rxISR1);
        _dmaActiveObjects[1] = this;
    }
    _dmaRX->interruptAtCompletion();

    // We may be using settings chain here so lets set it up.
    // Now lets setup TX chain.  Note if trigger TX is not set
    // we need to have the RX do it for us.
    _dmaTX->disable();
    _dmaTX->destination((volatile uint8_t &)*_shiftBufOutReg);
    _dmaTX->disableOnCompletion();

    _dmaTX->triggerAtHardwareEvent(_flexIO->getFlexIOHandler()->shiftersDMAChannel(_tx_shifter));

    _dma_state = DMAState::idle; // Should be first thing set!
    return true;
}

//=========================================================================
// Main Async Transfer function
//=========================================================================
#ifdef DEBUG_DMA_TRANSFERS
void dumpDMA_TCD(DMABaseClass *dmabc) {
    Serial4.printf("%x %x:", (uint32_t)dmabc, (uint32_t)dmabc->TCD);

    Serial4.printf("SA:%x SO:%d AT:%x NB:%x SL:%d DA:%x DO: %d CI:%x DL:%x CS:%x BI:%x\n", (uint32_t)dmabc->TCD->SADDR,
                   dmabc->TCD->SOFF, dmabc->TCD->ATTR, dmabc->TCD->NBYTES, dmabc->TCD->SLAST, (uint32_t)dmabc->TCD->DADDR,
                   dmabc->TCD->DOFF, dmabc->TCD->CITER, dmabc->TCD->DLASTSGA, dmabc->TCD->CSR, dmabc->TCD->BITER);
}
#endif

bool TeensyFlexSPI::transfer(const void *buf, void *retbuf, size_t count, EventResponderRef event_responder) {
    if (_dma_state == DMAState::notAllocated) {
        if (!initDMAChannels())
            return false;
    }

    if (_dma_state == DMAState::active)
        return false; // already active

    event_responder.clearEvent(); // Make sure it is not set yet
    if (count < 2) {
        // Use non-async version to simplify cases...
        transfer(buf, retbuf, count);
        event_responder.triggerEvent();
        return true;
    }

    // Now handle the cases where the count > then how many we can output in one DMA request
    if (count > MAX_DMA_COUNT) {
        _dma_count_remaining = count - MAX_DMA_COUNT;
        count = MAX_DMA_COUNT;
    } else {
        _dma_count_remaining = 0;
    }

    // Now See if caller passed in a source buffer.
    uint8_t *write_data = (uint8_t *)buf;
    if (buf) {
        _dmaTX->sourceBuffer((uint8_t *)write_data, count);
        _dmaTX->TCD->SLAST = 0; // Finish with it pointing to next location
        if ((uint32_t)write_data >= 0x20200000u)
            arm_dcache_flush(write_data, count);
    } else {
        _dmaTX->source((uint8_t &)_transferWriteFill); // maybe have setable value
        _dmaTX->transferCount(count);
    }
    _dmaTX->transferSize(_nTransferBytes);

    if (retbuf) {
        // On T3.5 must handle SPI1/2 differently as only one DMA channel
        _dmaRX->destinationBuffer((uint8_t *)retbuf, count);
        _dmaRX->TCD->DLASTSGA = 0; // At end point after our bufffer
        if ((uint32_t)retbuf >= 0x20200000u)
            arm_dcache_delete(retbuf, count);
    } else {
        // Write  only mode
        _dmaRX->destination((uint8_t &)bit_bucket);
        _dmaRX->transferCount(count);
    }
    _dmaRX->transferSize(_nTransferBytes);

    _dma_event_responder = &event_responder;
    // Now try to start it?
    // Setup DMA main object
    yield();

#ifdef DEBUG_DMA_TRANSFERS
    // Lets dump TX, RX
    dumpDMA_TCD(_dmaTX);
    dumpDMA_TCD(_dmaRX);
#endif

    // Lets turn on the DMA handling for this
    _flexIO->getFlexIO()->SHIFTSDEN |= SHIFTER_MASK(_rx_shifter) | SHIFTER_MASK(_tx_shifter);

    _dmaRX->enable();
    _dmaTX->enable();

    _dma_state = DMAState::active;
    return true;
}

void TeensyFlexSPI::_dma_rxISR0(void) {
    TeensyFlexSPI::_dmaActiveObjects[0]->dma_rxisr();
}

void TeensyFlexSPI::_dma_rxISR1(void) {
    TeensyFlexSPI::_dmaActiveObjects[1]->dma_rxisr();
}

//-------------------------------------------------------------------------
// DMA RX ISR
//-------------------------------------------------------------------------
void TeensyFlexSPI::dma_rxisr(void) {
    _dmaRX->clearInterrupt();
    _dmaTX->clearComplete();
    _dmaRX->clearComplete();

    if (_dma_count_remaining) {
        // What do I need to do to start it back up again...
        // We will use the BITR/CITR from RX as TX may have prefed some stuff
        if (_dma_count_remaining > MAX_DMA_COUNT) {
            _dma_count_remaining -= MAX_DMA_COUNT;
        } else {
            _dmaTX->transferCount(_dma_count_remaining);
            _dmaRX->transferCount(_dma_count_remaining);

            _dma_count_remaining = 0;
        }
        _dmaRX->enable();
        _dmaTX->enable();
    } else {

        _flexIO->getFlexIO()->SHIFTSDEN &= ~(SHIFTER_MASK(_rx_shifter) | SHIFTER_MASK(_tx_shifter)); // turn off DMA on both RX and TX
        _dma_state = DMAState::completed;                                   // set back to 1 in case our call wants to start up dma again
        _dma_event_responder->triggerEvent();
    }
}