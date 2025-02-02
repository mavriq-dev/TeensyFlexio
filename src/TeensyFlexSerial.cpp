/* Based on the wonderful work of https://github.com/KurtE Many Many Thanks!
 * The repository is at https://github.com/KurtE/FlexIO_t4
 */

#include "TeensyFlexSerial.h"
#include "imxrt.h"
#include "FlexIO_t4.h"

void TeensyFlexSerial::begin(uint32_t baud, uint16_t format) {
    Serial.printf("Setup FlexIO monitor\n");
    Serial.printf(" TX PIN: %d, TX Flex Number: %d\n", _tx_pin, _tx_flex_number);
    if (_tx_pin != -1 && _tx_flex_number != -1) {
        Serial.printf("Initializing TX\n");
        _tx_flexio = TeensyFlexIO();
        _tx_flexio.begin(static_cast<TeensyFlexIO::FlexIOModule>(_tx_flex_number -1));
        _tx_timer = _tx_flexio.requestTimer(_tx_timer);
        _tx_shifter = _tx_flexio.requestShifter(_tx_shifter);

        ShifterConfig txConfig;
        txConfig.mode = ShifterMode::Transmit;
        txConfig.pinSelect = _tx_pin;
        txConfig.pinConfig = PinConfig::Output; 
        txConfig.startBit = 2;  
        txConfig.stopBit = 3;   
        txConfig.timerSelect = _tx_timer;
        _tx_flexio.configureShifter(_tx_shifter, txConfig); 

        TimerConfig timerConfig;

        float tx_clock_speed = _tx_flexio.getFlexIOHandler()->computeClockRate();
        uint32_t tx_baud_div = roundf(tx_clock_speed / (float)(baud * 2));
        if (tx_baud_div > 255) tx_baud_div = 255;
        timerConfig.mode = TimerMode::Baud;
        timerConfig.pinPolarity = PinPolarity::ActiveHigh;
        timerConfig.pinSelect = _tx_pin;
        timerConfig.pinConfig = PinConfig::Disabled; 
        timerConfig.triggerSource = TriggerSource::Internal;
        timerConfig.triggerPolarity = TriggerPolarity::ActiveLow;
        timerConfig.triggerSelect = _tx_flexio.calculateTriggerSelect(TriggerType::SHIFTER, 0);
        timerConfig.startBit = 1;
        timerConfig.stopBit = 2;
        timerConfig.timerEnable = TimerEnable::TriggerHigh;

        timerConfig.timerDisable = TimerDisable::OnCompare;
        timerConfig.timerReset = TimerReset::Never;
        timerConfig.timerDecrement = TimerDecrement::FlexIOClock;
        timerConfig.timerOutput = TimerOutput::One;
        timerConfig.asDual().bits_in_word = 0xF;
        timerConfig.asDual().baud_rate_div = tx_baud_div;
        _tx_flexio.configureTimer(_tx_timer, timerConfig);

        __disable_irq();
        asm volatile("dsb");
        _tx_flexio.enable();
        _tx_flexio.disableShifterInterrupt(_tx_shifter);
        asm volatile("dsb");
        __enable_irq();

        _tx_flexio.setPinFlexioMode(_tx_pin);
        _tx_flexio.getFlexIOHandler()->addIOHandlerCallback(this);
    }

    if (_rx_pin != -1 && _rx_flex_number != -1) {
        Serial.printf("Initializing RX\n");
        _rx_lexio = TeensyFlexIO();
        _rx_lexio.begin(static_cast<TeensyFlexIO::FlexIOModule>(_rx_flex_number -1));

        _rx_timer = _rx_lexio.requestTimer(_rx_timer);
        _rx_shifter = _rx_lexio.requestShifter(_rx_shifter);

        ShifterConfig rxConfig;
        rxConfig.mode = ShifterMode::Receive;
        rxConfig.pinSelect = _rx_pin;
        rxConfig.pinPolarity = PinPolarity::ActiveHigh;
        rxConfig.timerPolarity = TimerPolarity::ActiveLow;
        rxConfig.startBit = 2;  
        rxConfig.stopBit = 3;   
        rxConfig.timerSelect = _rx_timer;
        _rx_lexio.configureShifter(_rx_shifter, rxConfig);  

        TimerConfig timerConfig;

        float rx_clock_speed = _rx_lexio.getFlexIOHandler()->computeClockRate();
        uint32_t rx_baud_div = roundf(rx_clock_speed / (float)(baud * 2));
        if (rx_baud_div > 255) rx_baud_div = 255;
        timerConfig.mode = TimerMode::Baud;
        timerConfig.pinPolarity = PinPolarity::ActiveLow;
        timerConfig.pinSelect = _rx_pin;
        timerConfig.startBit = 1;
        timerConfig.stopBit = 2;
        timerConfig.timerEnable = TimerEnable::PinRising;
        timerConfig.timerDisable = TimerDisable::OnCompare;
        timerConfig.timerReset = TimerReset::TriggerRising;
        timerConfig.timerOutput = TimerOutput::EnableAndReset;
        timerConfig.asDual().bits_in_word = 0xF;
        timerConfig.asDual().baud_rate_div = rx_baud_div;
        _rx_lexio.configureTimer(_rx_timer, timerConfig);
        _rx_lexio.enable();

        _rx_lexio.setPinFlexioMode(_rx_pin);
        _rx_lexio.getFlexIOHandler()->addIOHandlerCallback(this);

        __disable_irq();
        asm volatile("dsb");
        _rx_lexio.enableShifterInterrupt(_rx_shifter);
        asm volatile("dsb");
        __enable_irq();
    }

    delay(1);
    #if defined(DEBUG) && defined(DEBUG_TEENSYFLEXSERIAL)
        printDebugInfo();
    #endif
}

TeensyFlexSerial::~TeensyFlexSerial() {
	if (_tx_flexio.isInitialized()) {
		delete _tx_flexio.getFlexIO();
        delete _tx_flexio.getFlexIOHandler();
	}
    if (_rx_lexio.isInitialized()) {
        delete _rx_lexio.getFlexIO();
        delete _rx_lexio.getFlexIOHandler();
    }
}

void TeensyFlexSerial::flush(void) {
    if(!_tx_flexio.isInitialized()) return;
	uint32_t start_time = millis();
	while (_transmitting) {
		if ((millis()-start_time) > FLUSH_TIMEOUT) {
			return;
		}
		yield(); // wait
	}
}


int TeensyFlexSerial::available(void) {
    if(!_rx_lexio.isInitialized()) return -1;

	uint32_t head, tail;

	head = _rx_buffer_head;
	tail = _rx_buffer_tail;
	if (head >= tail) return head - tail;
    
	return RX_BUFFER_SIZE - tail + head;
}

int TeensyFlexSerial::peek(void) {
    if(!_rx_lexio.isInitialized()) return -1;
	if (_rx_buffer_head == _rx_buffer_tail) return -1;
	return _rx_buffer[_rx_buffer_tail] ;
}

int TeensyFlexSerial::read(void) {
    if(!_rx_lexio.isInitialized()) return -1;
	int return_value = -1;
	if (_rx_buffer_head != _rx_buffer_tail) {
		return_value = _rx_buffer[_rx_buffer_tail++] ;
		if (_rx_buffer_tail >= RX_BUFFER_SIZE) 
			_rx_buffer_tail = 0;
	}

	return return_value;
}

void TeensyFlexSerial::clear(void){

}

int TeensyFlexSerial::availableForWrite(void) {
    if(!_tx_flexio.isInitialized()) return -1;
	uint32_t head, tail;

	head = _tx_buffer_head;
	tail = _tx_buffer_tail;
	if (head >= tail) return TX_BUFFER_SIZE - 1 - head + tail;
	return tail - head - 1;
}

bool TeensyFlexSerial::call_back(FlexIOHandler *pflex) {
    if(pflex == _rx_lexio.getFlexIOHandler()){
        // Serial.printf("RX callback\n");
        if (SHIFT_STAT(_rx_lexio) & SHIFTER_MASK(_rx_shifter)) {
  			uint8_t c = _rx_lexio.getFlexIOHandler()->port().SHIFTBUFBYS[_rx_shifter] & 0xff;
			uint32_t head;
			head = _rx_buffer_head;
			if (++head >= RX_BUFFER_SIZE) head = 0;
			// don't save char if buffer is full...
			if (_rx_buffer_tail != head) {
				_rx_buffer[_rx_buffer_head] = c;
				_rx_buffer_head = head;
			}
		}
    }

    if(pflex == _tx_flexio.getFlexIOHandler()){
        // Check if shifter is ready and interrupt is enabled
        if ((SHIFT_STAT(_tx_flexio) & SHIFTER_MASK(_tx_shifter)) && (SHIFT_SIEN(_tx_flexio) & SHIFTER_MASK(_tx_shifter))) {
            if (_tx_buffer_head != _tx_buffer_tail) {
                // Write next byte from buffer to shifter
                SHIFT_BUFFER(_tx_flexio, _tx_shifter) = _tx_buffer[_tx_buffer_tail++];
                if (_tx_buffer_tail >= TX_BUFFER_SIZE) {
                    _tx_buffer_tail = 0;
                }
            }
            
            // If buffer is empty, disable shifter interrupt and enable timer
            if (_tx_buffer_head == _tx_buffer_tail) {
                __disable_irq();
                asm volatile("dsb");
                _tx_flexio.disableShifterInterrupt(_tx_shifter);
                _tx_flexio.enableTimerInterrupt(_tx_timer); 
                _tx_flexio.clearTimerStatus(_tx_timer);
                asm volatile("dsb");
                __enable_irq();
            }
        }
        
        // Handle timer interrupt (transmission complete)
        if (TIME_IEN(_tx_flexio) & TIME_STAT(_tx_flexio) & TIMER_MASK(_tx_timer)) {
            __disable_irq();
            asm volatile("dsb");
            if (_transmitting >= 2) {
                _tx_flexio.disableTimerInterrupt(_tx_timer);
                _transmitting = 0;
            } else {
                _transmitting++;
            }
            _tx_flexio.clearTimerStatus(_tx_timer);
            asm volatile("dsb");
            __enable_irq();
        }
        
        // Handle error condition
        if (SHIFT_ERR(_tx_flexio) & SHIFTER_MASK(_tx_shifter)) {
            __disable_irq();
            asm volatile("dsb");
            _tx_flexio.clearShifterError(_tx_shifter);
            _tx_flexio.clearShifterStatus(_tx_shifter);
            if (_tx_buffer_head == _tx_buffer_tail) {
                _tx_flexio.disableShifterInterrupt(_tx_shifter);
            }
            asm volatile("dsb");
            __enable_irq();
        }
    }

    return false;  // Return false to allow other callbacks to run
}

// Move our writeChar method here
size_t TeensyFlexSerial::write(uint8_t c) {
    uint32_t head = _tx_buffer_head;
    if (++head >= TX_BUFFER_SIZE) {
        head = 0;
    }

    // Wait if buffer is full
    while (_tx_buffer_tail == head) {
        Serial.printf("Buffer full, waiting... (head=%d, tail=%d)\n", head, _tx_buffer_tail);
        yield();
    }

    // Store character and update state
    _tx_buffer[_tx_buffer_head] = c;
    __disable_irq();
    asm volatile("dsb");
    _transmitting = 1;
    _tx_buffer_head = head;
    
    // Enable shifter interrupt
    uint32_t timer_mask = (1 << _tx_timer);
    SHIFT_SIEN(_tx_flexio) |= SHIFTER_MASK(_tx_shifter);  // enable interrupt on this shifter
    TIME_IEN(_tx_flexio) &= ~timer_mask;      // disable timer interrupt
    TIME_STAT(_tx_flexio) = timer_mask;       // clear timer status
    asm volatile("dsb");
    __enable_irq();
    return 1;
}

float TeensyFlexSerial::setClock(float frequency){
    float freqout=0;
    if (_tx_flexio.isInitialized() && _rx_lexio.isInitialized() && 
        !_rx_lexio.getFlexIOHandler()->usesSameClock(_tx_flexio.getFlexIOHandler())) {
        freqout = _tx_flexio.setClock(frequency);
        freqout = _rx_lexio.setClock(frequency);
    } else {
        if (_tx_flexio.isInitialized()) freqout = _tx_flexio.setClock(frequency);
        if (_rx_lexio.isInitialized()) freqout = _rx_lexio.setClock(frequency);
    }
    return freqout;
}

float TeensyFlexSerial::setClockUsingAudioPLL(float frequency){
	float freqout=0;
	if (_tx_flexio.isInitialized() && _rx_lexio.isInitialized() && 
        !_rx_lexio.getFlexIOHandler()->usesSameClock(_tx_flexio.getFlexIOHandler())) {
		freqout = _tx_flexio.setClockUsingAudioPLL(frequency);
		freqout = _rx_lexio.setClockUsingAudioPLL(frequency);
	} else {
		if (_tx_flexio.isInitialized()) freqout = _tx_flexio.setClockUsingAudioPLL(frequency);
		if (_rx_lexio.isInitialized()) freqout = _rx_lexio.setClockUsingAudioPLL(frequency);
	}
	return freqout;
}

float TeensyFlexSerial::setClockUsingVideoPLL(float frequency){
	float freqout=0;
	if (_tx_flexio.isInitialized() && _rx_lexio.isInitialized() && 
        !_rx_lexio.getFlexIOHandler()->usesSameClock(_tx_flexio.getFlexIOHandler())) {
		freqout = _tx_flexio.setClockUsingVideoPLL(frequency);
		freqout = _rx_lexio.setClockUsingVideoPLL(frequency);
	} else {
		if (_tx_flexio.isInitialized()) freqout = _tx_flexio.setClockUsingVideoPLL(frequency);
		if (_rx_lexio.isInitialized()) freqout = _rx_lexio.setClockUsingVideoPLL(frequency);
	}
	return freqout;
}

void TeensyFlexSerial::printDebugInfo() {
    if(_tx_flexio.isInitialized()) { 
        uint32_t shiftstat = _tx_flexio.getFlexIO()->SHIFTSTAT;
        uint32_t txshiftctl = _tx_flexio.getFlexIO()->SHIFTCTL[_tx_shifter];
        uint32_t txshiftcfg = _tx_flexio.getFlexIO()->SHIFTCFG[_tx_shifter];
        uint32_t ctrl = _tx_flexio.getFlexIO()->CTRL;
        uint32_t txTimstat = _tx_flexio.getFlexIO()->TIMSTAT;  // Add timer status
        uint32_t txtimctl = _tx_flexio.getFlexIO()->TIMCTL[_tx_timer];  // Add timer control
        uint32_t txtimcfg = _tx_flexio.getFlexIO()->TIMCFG[_tx_timer];  // Add timer config
        
        Serial.printf("\nFlexIO CTRL: 0x%08X FlexIO Clock enabled: %d, CCM_CCGR5: 0x%08X\n", ctrl, (CCM_CCGR5 & (3 << 2)) == (3 << 2), CCM_CCGR5);
        Serial.printf("TX Shifter # %d: , TX timer # %d:\n", _tx_shifter, _tx_timer);
        Serial.printf("TX SHIFTCTL: 0x%08X, SHIFTCFG: 0x%08X\n", txshiftctl, txshiftcfg);
        Serial.printf("SHIFTSTAT: 0x%08X, SHIFTERR: 0x%08X\n", shiftstat, _tx_flexio.getFlexIO()->SHIFTERR);
        Serial.printf("Buffer ready TX: %d\n", _tx_flexio.bufferReady(_tx_shifter));
        Serial.printf("TX TIMCTL: 0x%08X, TIMCFG: 0x%08X\n",  txtimctl, txtimcfg);
        Serial.printf("TIMSTAT: 0x%08X, TIMCMP: 0x%08X\n", txTimstat, _tx_flexio.getFlexIO()->TIMCMP[_tx_timer]);
        Serial.printf("SHIFTSIEN: 0x%08X, TIMIEN: 0x%08X\n",_tx_flexio.getFlexIO()->SHIFTSIEN, _tx_flexio.getFlexIO()->TIMIEN);
        Serial.printf("IRQ_FLEXIO1: %d\n", IRQ_FLEXIO1);
        Serial.flush();
    }
    if(_rx_lexio.isInitialized()) {
        uint32_t shiftstat = _rx_lexio.getFlexIO()->SHIFTSTAT;
        uint32_t rxshiftctl = _rx_lexio.getFlexIO()->SHIFTCTL[_rx_shifter];
        uint32_t rxshiftcfg = _rx_lexio.getFlexIO()->SHIFTCFG[_rx_shifter];
        uint32_t ctrl = _rx_lexio.getFlexIO()->CTRL;
        uint32_t rxtimstat = _rx_lexio.getFlexIO()->TIMSTAT;  // Add timer status
        uint32_t rxtimctl = _rx_lexio.getFlexIO()->TIMCTL[_rx_timer];  // Add timer control
        uint32_t rxtimcfg = _rx_lexio.getFlexIO()->TIMCFG[_rx_timer];  // Add timer config
        
        Serial.printf("\nFlexIO CTRL: 0x%08X FlexIO Clock enabled: %d, CCM_CCGR5: 0x%08X\n", ctrl, (CCM_CCGR5 & (3 << 2)) == (3 << 2), CCM_CCGR5);
        Serial.printf("RX Shifter # %d: , RX timer # %d:\n", _rx_shifter, _rx_timer);
        Serial.printf("RX SHIFTCTL: 0x%08X, RX SHIFTCFG: 0x%08X\n", rxshiftctl, rxshiftcfg);
        Serial.printf("SHIFTSTAT: 0x%08X, SHIFTERR: 0x%08X\n", shiftstat, _rx_lexio.getFlexIO()->SHIFTERR);
        Serial.printf("Buffer ready RX: %d\n", _rx_lexio.bufferReady(_rx_shifter));
        Serial.printf("RX TIMCTL: 0x%08X, RX TIMCFG: 0x%08X\n",  rxtimctl, rxtimcfg);
        Serial.printf("TIMSTAT: 0x%08X, TIMCMP: 0x%08X\n", rxtimstat, _rx_lexio.getFlexIO()->TIMCMP[_rx_timer]);
        Serial.printf("SHIFTSIEN: 0x%08X, TIMIEN: 0x%08X\n",_rx_lexio.getFlexIO()->SHIFTSIEN, _rx_lexio.getFlexIO()->TIMIEN);
        Serial.printf("IRQ_FLEXIO1: %d\n", IRQ_FLEXIO1);
        Serial.flush();
    }
}