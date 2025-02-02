#include "TeensyFlexIO.h"

void TeensyFlexIO::begin(FlexIOModule module) {
    Serial.printf("Initializing FlexIO %d\n", module);
    switch(module) {
        case FLEXIO1:
            _flexio = (IMXRT_FLEXIO_t*)&IMXRT_FLEXIO1_S;
            _flexio_handler = FlexIOHandler::flexIOHandler_list[0]; // Create handler for FlexIO1
            CCM_CCGR5 |= CCM_CCGR5_FLEXIO1(CCM_CCGR_ON);
            break;
        case FLEXIO2:
            _flexio = (IMXRT_FLEXIO_t*)&IMXRT_FLEXIO2_S;
            _flexio_handler = FlexIOHandler::flexIOHandler_list[1];  // Create handler for FlexIO2
            CCM_CCGR3 |= CCM_CCGR3_FLEXIO2(CCM_CCGR_ON);
            break;
        case FLEXIO3:
            _flexio = (IMXRT_FLEXIO_t*)&IMXRT_FLEXIO3_S;
            _flexio_handler = FlexIOHandler::flexIOHandler_list[2];  // Create handler for FlexIO3
            CCM_CCGR7 |= CCM_CCGR7_FLEXIO3(CCM_CCGR_ON);
            break;
    }

    _is_initialized = true;
}

TeensyFlexIO::~TeensyFlexIO() {
    _flexio_handler = nullptr;
    _flexio = nullptr;
    _is_initialized = false;
}

int8_t TeensyFlexIO::requestShifter(int8_t shifterNum) {

    // If specific shifter requested
    if (shifterNum >= 0) {
        // Check if the requested shifter is available
        bool shifterAvailable = _flexio_handler->claimShifter(shifterNum);
        if (shifterAvailable) {
            return shifterNum;
        }
        return -1;  // Requested shifter not available
    }

    // No specific shifter requested, get first available
    return _flexio_handler->requestShifter();
}

void TeensyFlexIO::releaseShifter(uint8_t shifterIndex) {
    _flexio_handler->freeShifter(shifterIndex);
}

uint8_t TeensyFlexIO::shiftersDMAChannel(uint8_t n){
    return _flexio_handler->shiftersDMAChannel(n);
}

int8_t TeensyFlexIO::requestTimer(int8_t timerNum) {
    // If specific timer requested
    if (timerNum >= 0) {
        // Check if the requested timer is available
        bool timerAvailable = _flexio_handler->claimTimer(timerNum);
        if (timerAvailable) {
            return timerNum;
        }
        return -1;  // Requested timer not available
    }

    // No specific timer requested, get first available
    return _flexio_handler->requestTimers();
}

void TeensyFlexIO::releaseTimer(uint8_t timerIndex) {
    _flexio_handler->freeTimers(timerIndex);
}

int8_t TeensyFlexIO::requestTimers(uint8_t cnt){
    return _flexio_handler->requestTimers(cnt);
}

void TeensyFlexIO::configureShifter(uint8_t shifterIndex, const ShifterConfig& config) {
    configureShifter(shifterIndex, 
        static_cast<uint8_t>(config.mode),
        config.pinSelect,
        static_cast<uint8_t>(config.pinPolarity),
        static_cast<uint8_t>(config.pinConfig),
        static_cast<uint8_t>(config.inputSource),
        config.timerSelect,
        static_cast<uint8_t>(config.timerPolarity),
        config.parallelWidth,
        config.startBit,
        config.stopBit
    );
}

void TeensyFlexIO::configureShifter(uint8_t shifterIndex, uint8_t mode, uint8_t pinSelect, 
    uint8_t pinPolarity, uint8_t pinConfig, uint8_t inputSource, uint8_t timerSelect, 
    uint8_t timerPolarity, uint8_t parallelWidth, uint8_t startBit, uint8_t stopBit) {

    String msg = "";


    if (shifterIndex >= FlexIOHandler::CNT_SHIFTERS-1) {
         msg += "Shifter index can only be 0 to 7\n";
    } else if (mode > 7) {
         msg += "Mode out of range\n";
    } else if (_flexio_handler->FlexIOIndex() == 0  && pinSelect > 15) {
        msg += "FlexIO1 can only select FXIO_D0 to FXIO_D15 \n";
    } else if (pinSelect > 31) {
        msg += "Pin select can only be 0 to 31\n"; 
    } else if (pinPolarity > 1) {
        msg += "Pin polarity can only be 0 or 1\n";
    } else if (pinConfig > 3) {
         msg += "Pin config can only be 0 to 3\n";
    } else if (inputSource > 1) {
        msg += "Input source can only be 0 (pin) or 1 (shifter)\n";
    } else if (timerSelect > 7) {
         msg += "Timer select can only be 0 to 7\n";
    } else if (timerPolarity > 1) {
         msg += "Timer polarity can only be 0 or 1\n";
    } else if (parallelWidth > 15) {
        msg += "Width can only be 0 to 15\n";
    } else if (startBit > 3) {
         msg += "Start bit can only be 0 to 3\n";
    } else if (stopBit > 3) {
        msg += "Stop bit can only be 0 to 3\n";
    }

    if (!msg.equals("")) {
        #ifdef DEBUG && DEBUG_FLEXIO
            Serial.println(msg);
        #endif
        return;
    }

    uint8_t flex_pin = _flexio_handler->mapIOPinToFlexPin(pinSelect);

    if (flex_pin == 255 && pinSelect == 0) { flex_pin = 0; };
    Serial.printf("pinSelect: %d, flex_pin: %d\n", pinSelect, flex_pin);

    // _flexio->SHIFTSTAT = 1 << shifterIndex; 
    // _flexio->SHIFTERR = 1 << shifterIndex;

    // Set up the shifter configuration register (SHIFTCFG)
    uint32_t shiftcfg = 0;
    shiftcfg |= FLEXIO_SHIFTCFG_PWIDTH((parallelWidth));     // PWIDTH: Parallel parallelWidth
    shiftcfg |= (inputSource ? FLEXIO_SHIFTCFG_INSRC : 0); // INSRC: Input source
    shiftcfg |= FLEXIO_SHIFTCFG_SSTOP(stopBit);          // SSTOP: Stop bit
    shiftcfg |= FLEXIO_SHIFTCFG_SSTART(startBit);        // SSTART: Start bit
    _flexio->SHIFTCFG[shifterIndex] = shiftcfg;

    // Set up the shifter control register (SHIFTCTL)
    uint32_t shiftctl = 0;
    shiftctl |= FLEXIO_SHIFTCTL_TIMSEL(timerSelect);     // TIMSEL: Timer select
    shiftctl |= FLEXIO_SHIFTCTL_PINSEL(flex_pin);       // PINSEL: Pin select
    shiftctl |= (pinPolarity ? FLEXIO_SHIFTCTL_PINPOL : 0); // PINPOL: Pin polarity
    shiftctl |= FLEXIO_SHIFTCTL_PINCFG(pinConfig);       // PINCFG: Pin configuration
    shiftctl |= (timerPolarity ? FLEXIO_SHIFTCTL_TIMPOL : 0); // TIMPOL: Timer polarity
    shiftctl |= FLEXIO_SHIFTCTL_SMOD(mode);              // SMOD: Shifter mode
    _flexio->SHIFTCTL[shifterIndex] = shiftctl;

}

void TeensyFlexIO::configureTimer(uint8_t timerIndex, uint8_t mode, uint8_t pinSelect, 
    uint8_t pinPolarity, uint8_t pinConfig, uint8_t triggerSource, int8_t triggerSelect, 
    uint8_t triggerPolarity, uint8_t timerEnable, uint8_t timerDisable, uint8_t timerReset, 
    uint8_t timerDecrement, uint8_t timerOutput, uint8_t startBit, uint8_t stopBit, 
    uint8_t compHigh, uint8_t compLow) {
    
    String msg = "";

    if (timerIndex >= FlexIOHandler::CNT_TIMERS) {
        msg += "Timer index can only be 0 to 7\n";
    } else if (mode > 3) {
        msg += "Mode out of range\n";
    } else if (_flexio_handler->FlexIOIndex() == 0 && pinSelect > 15) {
       msg += "FlexIO1 can only select FXIO_D0 to FXIO_D15\n";
    } else if (pinSelect > 31) {
        msg += "Pin select can only be 0 to 31\n";
    } else if (pinPolarity > 1) {
         msg += "Pin polarity can only be 0 or 1\n";
    } else if (pinConfig > 3) {
        msg += "Pin config can only be 0 to 3\n";
    } else if (triggerSource > 1) {
        msg += "Trigger source can only be 0 or 1\n";
    } else if (triggerSelect > 31) {
         msg += "Trigger select must be 0 to 31\n";
    } else if (triggerPolarity > 1) {
        msg += "Trigger polarity can only be 0 or 1\n";
    } else if (timerEnable > 7) {
         msg += "Timer enable can only be 0 to 7\n";
    } else if (timerDisable > 6) {
        msg += "Timer disable can only be 0 to 7\n";
    } else if (timerReset > 7) {
        msg += "Timer reset can only be 0 to 7\n";
    } else if (timerDecrement > 3) {
        msg += "Timer decrement can only be 0 to 3\n";
    } else if (timerOutput > 3) {
         msg += "Timer output can only be 0 to 3\n";
    } else if (startBit > 1) {
         msg += "Start bit can only be 0 to 1\n";
    } else if (stopBit > 3) {
        msg += "Stop bit can only be 0 to 3\n";
    }else if (compHigh > 0xFF or compLow > 0XFF) {
        msg += "Compare high and low can only be 0 to 7\n";
    } else if (mode == 2 && compHigh > compLow) {
        msg += "In PWM mode, high time (compHigh) must not exceed period (compLow)\n";
    }

    if (!msg.equals("")) {
        #ifdef DEBUG && DEBUG_FLEXIO
            Serial.println(msg);
        #endif
        return;
    }

    uint8_t flex_pin = _flexio_handler->mapIOPinToFlexPin(pinSelect);

    if (flex_pin == 255 && pinSelect == 0) { flex_pin = 0; };

        // Set up the timer configuration register (TIMCFG)
    uint32_t timcfg = 0;
    timcfg |= FLEXIO_TIMCFG_TIMOUT(timerOutput);         // TIMOUT: Timer output
    timcfg |= FLEXIO_TIMCFG_TIMDEC(timerDecrement);      // TIMDEC: Timer decrement
    timcfg |= FLEXIO_TIMCFG_TIMRST(timerReset);          // TIMRST: Timer reset
    timcfg |= FLEXIO_TIMCFG_TIMDIS(timerDisable);        // TIMDIS: Timer disable
    timcfg |= FLEXIO_TIMCFG_TIMENA(timerEnable);         // TIMENA: Timer enable
    timcfg |= FLEXIO_TIMCFG_TSTOP(stopBit);              // TSTOP: Timer stop bit
    timcfg |= (startBit ? FLEXIO_TIMCFG_TSTART : 0);     // TSTART: Timer start bit
    _flexio->TIMCFG[timerIndex] = timcfg;

    // Set up the timer control register (TIMCTL)
    uint32_t timctl = 0;
    timctl |= FLEXIO_TIMCTL_TRGSEL(triggerSelect);       // TRGSEL: Trigger select
    timctl |= (triggerPolarity ? FLEXIO_TIMCTL_TRGPOL : 0); // TRGPOL: Trigger polarity
    timctl |= (triggerSource ? FLEXIO_TIMCTL_TRGSRC : 0); // TRGSRC: Trigger source
    timctl |= FLEXIO_TIMCTL_PINCFG(pinConfig);           // PINCFG: Pin configuration
    timctl |= FLEXIO_TIMCTL_PINSEL(flex_pin);           // PINSEL: Pin select
    timctl |= (pinPolarity ? FLEXIO_TIMCTL_PINPOL : 0);  // PINPOL: Pin polarity
    timctl |= FLEXIO_TIMCTL_TIMOD(mode);                 // TIMOD: Timer mode
    _flexio->TIMCTL[timerIndex] = timctl;



    // Set up timer compare register (TIMCMP)
    uint32_t timcmp = (compHigh << 8) | compLow;
    _flexio->TIMCMP[timerIndex] = timcmp;

    // Clear any pending flags for this timer
    // _flexio->TIMSTAT = 1 << timerIndex;
}

void TeensyFlexIO::configureTimer(uint8_t timerIndex, const TimerConfig& config) {
    // Validate timer index
    if (timerIndex >= FlexIOHandler::CNT_TIMERS) {
        return;
    }

    // Validate pin select based on FlexIO module
    if (_flexio_handler->FlexIOIndex() == 0 && config.pinSelect > 15) {
        return;
    }
    if (config.pinSelect > 31) {
        return;
    }

    uint8_t compHigh = 0;
    uint8_t compLow = 0;

    // Set up compare values based on mode
    switch (config.mode) {
        case TimerMode::SingleCounter:
            compHigh = config.counter.compareValue;
            compLow = config.counter.reloadValue;
            break;
        case TimerMode::Baud:
            compHigh = config.dual.bits_in_word;
            compLow = config.dual.baud_rate_div;
            break;
        case TimerMode::PWM:
            // In PWM mode, high time must not exceed period
            if (config.pwm.highPeriod > config.pwm.lowPeriod) {
                return;
            }
            compHigh = config.pwm.highPeriod;
            compLow = config.pwm.lowPeriod;
            break;
        default:
            compHigh = config.compHigh;
            compLow = config.compLow;
            break;
    }

    configureTimer(timerIndex, 
        static_cast<uint8_t>(config.mode),
        config.pinSelect,
        static_cast<uint8_t>(config.pinPolarity),
        static_cast<uint8_t>(config.pinConfig),
        static_cast<uint8_t>(config.triggerSource),
        static_cast<uint8_t>(config.triggerSelect),
        static_cast<uint8_t>(config.triggerPolarity),
        static_cast<uint8_t>(config.timerEnable),
        static_cast<uint8_t>(config.timerDisable),
        static_cast<uint8_t>(config.timerReset),
        static_cast<uint8_t>(config.timerDecrement),
        static_cast<uint8_t>(config.timerOutput),
        config.startBit,
        config.stopBit,
        compHigh,
        compLow);
}

TimerConfig TeensyFlexIO::readTimerConfig(uint8_t timerIndex) {
    TimerConfig config;

    // Return default config if timer index is invalid
    if (timerIndex >= FlexIOHandler::CNT_TIMERS) {
        return config;
    }

    // Read timer control register (TIMCTL)
    uint32_t timctl = _flexio->TIMCTL[timerIndex];
    config.triggerSelect = (timctl >> 24) & 0x3F;
    config.triggerPolarity = static_cast<TriggerPolarity>((timctl >> 23) & 0x1);
    config.triggerSource = static_cast<TriggerSource>((timctl >> 22) & 0x1);
    config.pinConfig = static_cast<PinConfig>((timctl >> 16) & 0x3);
    config.pinSelect = (timctl >> 8) & 0x1F;
    config.pinPolarity = static_cast<PinPolarity>((timctl >> 7) & 0x1);
    config.mode = static_cast<TimerMode>(timctl & 0x7);

    // Read timer configuration register (TIMCFG)
    uint32_t timcfg = _flexio->TIMCFG[timerIndex];
    config.timerOutput = static_cast<TimerOutput>((timcfg >> 24) & 0x3);
    config.timerDecrement = static_cast<TimerDecrement>((timcfg >> 20) & 0x3);
    config.timerReset = static_cast<TimerReset>((timcfg >> 16) & 0x7);
    config.timerDisable = static_cast<TimerDisable>((timcfg >> 12) & 0x7);
    config.timerEnable = static_cast<TimerEnable>((timcfg >> 8) & 0x7);
    config.stopBit = (timcfg >> 4) & 0x7;
    config.startBit = (timcfg >> 1) & 0x1;

    // Read timer compare register (TIMCMP)
    uint32_t timcmp = _flexio->TIMCMP[timerIndex];
    uint8_t compHigh = (timcmp >> 8) & 0xFF;
    uint8_t compLow = timcmp & 0xFF;

    // Set mode-specific compare values
    switch (config.mode) {
        case TimerMode::SingleCounter:
            config.counter.compareValue = compHigh;
            config.counter.reloadValue = compLow;
            break;
        case TimerMode::Baud:
            config.dual.bits_in_word = compHigh;
            config.dual.baud_rate_div = compLow;
            break;
        case TimerMode::PWM:
            config.pwm.highPeriod = compHigh;
            config.pwm.lowPeriod = compLow;
            break;
        default:
            config.compHigh = compHigh;
            config.compLow = compLow;
            break;
    }

    return config;
}

ShifterConfig TeensyFlexIO::readShifterConfig(uint8_t shifterIndex) {
    ShifterConfig config;
    
    if (!_flexio || shifterIndex >= FlexIOHandler::CNT_SHIFTERS-1) {
        return config; // Return default config if invalid
    }

    // Read SHIFTCTL register
    uint32_t shiftctl = _flexio->SHIFTCTL[shifterIndex];
    config.timerSelect = (shiftctl >> 24) & 0x7;     // TIMSEL
    config.timerPolarity = static_cast<TimerPolarity>((shiftctl >> 23) & 0x1); // TIMPOL
    config.pinConfig = static_cast<PinConfig>((shiftctl >> 16) & 0x3);       // PINCONFIG
    config.pinSelect = (shiftctl >> 8) & 0x1F;      // PINSEL
    config.pinPolarity = static_cast<PinPolarity>((shiftctl >> 7) & 0x1);   // PINPOL
    config.mode = static_cast<ShifterMode>(shiftctl & 0x7);     
                
    // Read SHIFTCFG register
    uint32_t shiftcfg = _flexio->SHIFTCFG[shifterIndex];
    config.parallelWidth = ((shiftcfg >> 16) & 0x1F) ;    // PWIDTH
    config.inputSource = static_cast<InputSource>((shiftcfg >> 8) & 0x1);   // INSRC
    config.stopBit = (shiftcfg >> 4) & 0x3;               // SSTOP
    config.startBit = shiftcfg & 0x3;       // SSTART

    return config;
}


bool TeensyFlexIO::setPinFlexioMode(uint8_t pin) {
    uint8_t flex_pin = _flexio_handler->mapIOPinToFlexPin(pin);
    if (flex_pin == 0xFF) {  return false; } // 0xFF indicates invalid pin
    
    // Use the handler's built-in pin muxing function
    return _flexio_handler->setIOPinToFlexMode(pin);
}

bool TeensyFlexIO::setPinParameters(uint8_t pin, PullUp pullup, uint8_t driveStrength, uint8_t slewRate) {
    if (driveStrength > 7) return false;
    if (slewRate > 3) return false;

    uint32_t pad_config = IOMUXC_PAD_DSE(driveStrength) | IOMUXC_PAD_SPEED(slewRate);
    
    // Add pull-up/down configuration if requested
    if (pullup != PullUp::DISABLED) {
        pad_config |= IOMUXC_PAD_PKE | IOMUXC_PAD_PUE;
        pad_config |= IOMUXC_PAD_PUS(static_cast<uint8_t>(pullup));
    } 

    *(portControlRegister(pin)) = pad_config;
    return true;
}

float TeensyFlexIO::setClock(float frequency) {
    return _flexio_handler->setClock(frequency);
}

void TeensyFlexIO::setClockSettings(uint8_t clk_sel, uint8_t clk_pred, uint8_t clk_podf) {
    _flexio_handler->setClockSettings(clk_sel, clk_pred, clk_podf);
}

void TeensyFlexIO::disableShifterInterrupt(uint8_t shifter) {
    _flexio->SHIFTSIEN &= ~SHIFTER_MASK(shifter);  // Disable shifter interrupt
}

void TeensyFlexIO::enableShifterInterrupt(uint8_t shifter) {
    _flexio->SHIFTSIEN |= SHIFTER_MASK(shifter);  // Enable shifter interrupt
}

void TeensyFlexIO::disableTimerInterrupt(uint8_t timer) {
    _flexio->TIMIEN &= ~TIMER_MASK(timer);  
}

void TeensyFlexIO::enableTimerInterrupt(uint8_t timer) {
    _flexio->TIMIEN |= TIMER_MASK(timer);  // Enable timer interrupt
}

void TeensyFlexIO::clearShifterStatus(uint8_t shifter) {
    _flexio->SHIFTSTAT = SHIFTER_MASK(shifter);  // Disable shifter interrupt
}

void TeensyFlexIO::clearTimerStatus(uint8_t timer) {
    _flexio->TIMSTAT = TIMER_MASK(timer);   // Disable timer interrupt
}

void TeensyFlexIO::clearShifterError(uint8_t shifter) {
    _flexio->SHIFTERR = SHIFTER_MASK(shifter);   // clear error // Disable shifter interrupt
}


uint8_t TeensyFlexIO::calculateTriggerSelect(TriggerType type, uint8_t number) {
    switch(type) {
        case TriggerType::PIN:
            return 2 * number;  // 0,2,4,...
        case TriggerType::SHIFTER:
            return 4 * number + 1;  // 1,5,9,...
        case TriggerType::TIMER:
            return 4 * number + 3;  // 3,7,11,...
        default:
            return 0;
    }
}