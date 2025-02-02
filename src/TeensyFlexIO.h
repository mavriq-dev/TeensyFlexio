#ifndef _TEENSY_FLEX_IO_H_
#define _TEENSY_FLEX_IO_H_

#include <Arduino.h>
#include <FlexIO_t4.h>

#define SHIFTER_MASK(n) (1 << n)
#define TIMER_MASK(n) (1 << n)
#define SHIFT_BUFFER(flexio_obj,N) ((uint8_t *)(flexio_obj).getFlexIO()->SHIFTBUF)[N]
#define SHIFT_STAT(flexio_obj) ((flexio_obj).getFlexIO()->SHIFTSTAT)
#define SHIFT_ERR(flexio_obj) ((flexio_obj).getFlexIO()->SHIFTERR)
#define TIME_STAT(flexio_obj) ((flexio_obj).getFlexIO()->TIMSTAT)
#define TIME_IEN(flexio_obj) ((flexio_obj).getFlexIO()->TIMIEN)
#define SHIFT_SIEN(flexio_obj) ((flexio_obj).getFlexIO()->SHIFTSIEN)


    // Enums for shifter configuration
    /// Shifter mode configurations
    enum class ShifterMode {
        /// Shifter is disabled
        Disabled = 0,
        /// Receive data from external source
        Receive = 1,
        /// Transmit data to external source
        Transmit = 2,
        /// Match store mode
        MatchStore = 4,
        /// Match continuous mode
        MatchContinuous = 5,
        /// State mode
        State = 6,
        /// Logic mode
        Logic = 7
    };

    /// Pin polarity configurations
    enum class PinPolarity {
        /// Pin is active high
        ActiveHigh = 0,
        /// Pin is active low
        ActiveLow = 1
    };

    /// Pin configuration options
    enum class PinConfig {
        /// Pin is disabled
        Disabled = 0,
        /// Pin is open drain
        OpenDrain = 1,
        /// Pin is bidirectional
        Bidirectional = 2,
        /// Pin is output only
        Output = 3
    };

    /// Input source configurations
    enum class InputSource {
        /// Input from pin
        Pin = 0,
        /// Input from shifter
        Shifter = 1
    };

    /// Timer polarity configurations
    enum class TimerPolarity {
        /// Timer is positive edge triggered
        ActiveHigh = 0,
        /// Timer is negative edge triggered
        ActiveLow = 1
    };

    // Enums for timer configuration
    /// Timer mode configurations
    enum class TimerMode {
        /// Timer Disabled
        Disabled = 0,
        /// Dual 8-bit counters baud/bit
        Baud = 1,
        /// Dual 8-bit counters PWM
        PWM = 2,
        /// Single 16-bit counter
        SingleCounter = 3
    };

    /// Trigger source configurations
    enum class TriggerSource {
        /// External trigger
        External = 0,
        /// Internal trigger
        Internal = 1
    };

        enum class TriggerPolarity {
        /// Timer is positive edge triggered
        ActiveHigh = 0,
        /// Timer is negative edge triggered
        ActiveLow = 1
    };

    /// Timer enable configurations
    enum class TimerEnable {
        /// Timer always enabled
        Always = 0,
        /// Enable on timer N-1 enable
        N1Enable = 1,
        /// Enable on trigger high
        TriggerHigh = 2,
        /// Enable on trigger high and pin high
        TriggerHighPinHigh = 3,
        /// Enable on trigger pin rising edge
        PinRising = 4,
        /// Enable on pin rising edge and trigger high
        PinRisingTriggerHigh = 5,
        /// Enable on trigger rising edge
        TriggerRising = 6,
        /// Enable on trigger rising or falling edge
        TriggerRisingOrFalling = 7
    };

    /// Timer disable configurations
    enum class TimerDisable {
        /// Timer never disabled
        Never = 0,
        /// Disable on timer N-1 disable
        N1Disable = 1,
        /// Disable on timer compare (upper 8-bits match and decrement)
        OnCompare = 2,
        /// Disable on timer compare (upper 8-bits match and decrement) and trigger low
        OnCompareAndTriggerLow = 3,
        /// Disable on pin rising or falling edge
        PinRisingOrFalling = 4,
        /// Disable on pin rising or falling edge and trigger high
        PinRisingOrFallingAndTriggerHigh = 5,
        /// Disable on trigger falling edge
        TriggerFallingEdge = 6,
    };

    /// Timer reset configurations
    enum class TimerReset {
        /// Never reset
        Never = 0,
        /// Reset on timer pin equal to timer output
        PinEqualOutput = 1,
        /// Reset on timer trigger equal to timer output
        TriggerEqualOutput = 2,
        /// Reset on timer pin rising edge
        PinRising = 3,
        /// Reset on timer trigger rising edge
        TriggerRising = 4,
        /// Reset on timer trigger high
        TriggerHigh = 5,
        /// Reset on timer trigger low
        TriggerLow = 6,
        /// Reset on timer disable
        OnDisable = 7
    };

    /// Timer decrement configurations
    enum class TimerDecrement {
        /// Decrement on FlexIO clock
        FlexIOClock = 0,
        /// Decrement on trigger input
        TriggerInput = 1,
        /// Decrement on pin input
        PinInput = 2,
        /// Decrement on trigger input and pin input
        TriggerAndPin = 3
    };

    /// Timer output configurations
    enum class TimerOutput {
        /// Output is logic one
        One = 0,
        /// Output is logic zero
        Zero = 1,
        /// Output on timer enable and reset
        EnableAndReset = 2,
        /// Output is inverted on timer enable and reset
        InvertedEnableAndReset = 3
    };

    enum class TriggerType {
        PIN,
        SHIFTER,
        TIMER
    };

    // Structure to hold shifter configuration
    struct ShifterConfig {
        ShifterMode mode = ShifterMode::Disabled;
        uint8_t pinSelect = 0;        ///< Pin number (0-31, FlexIO1 only supports 0-15)
        PinPolarity pinPolarity = PinPolarity::ActiveHigh;
        PinConfig pinConfig = PinConfig::Disabled;
        InputSource inputSource = InputSource::Pin;
        uint8_t timerSelect = 0;      ///< Timer number (0-7)
        TimerPolarity timerPolarity = TimerPolarity::ActiveHigh;
        uint8_t parallelWidth = 0;            ///< Parallel width (1-16)
        uint8_t startBit = 0;         ///< Start bit value (0-3), behavior depends on mode
        uint8_t stopBit = 0;          ///< Stop bit value (0-3), behavior depends on mode
    };

    // Structure to hold timer configuration
    struct SingleCounterConfig {
        uint8_t compareValue;
        uint8_t reloadValue;
    };

    struct DualCounterConfig {
        uint8_t bits_in_word;
        uint8_t baud_rate_div;
    };

    struct PWMConfig {
        uint8_t highPeriod;
        uint8_t lowPeriod;
    };

    struct TimerConfig {
        TimerMode mode = TimerMode::Disabled;
        uint8_t pinSelect = 0;        ///< Pin number (0-31, FlexIO1 only supports 0-15)
        PinPolarity pinPolarity = PinPolarity::ActiveHigh;
        PinConfig pinConfig = PinConfig::Disabled;
        TriggerSource triggerSource = TriggerSource::External;
        uint8_t triggerSelect = 0;    ///< Trigger select (0-31)
        TriggerPolarity triggerPolarity = TriggerPolarity::ActiveHigh;
        TimerEnable timerEnable = TimerEnable::Always;
        TimerDisable timerDisable = TimerDisable::Never;
        TimerReset timerReset = TimerReset::Never;
        TimerDecrement timerDecrement = TimerDecrement::FlexIOClock;
        TimerOutput timerOutput = TimerOutput::One;
        uint8_t startBit = 0;         ///< Start bit (0-1)
        uint8_t stopBit = 0;          ///< Stop bit (0-3)
        union {
            struct {
                uint8_t compHigh;  // Generic names
                uint8_t compLow;
            };
            SingleCounterConfig counter;
            DualCounterConfig dual;
            PWMConfig pwm;
        };

        // Helper functions to get the right struct based on mode
        SingleCounterConfig& asCounter() { return counter; }
        DualCounterConfig& asDual() { return dual; }
        PWMConfig& asPWM() { return pwm; }
    };

    enum class PullUp {
        DISABLED = -1,
        PULLDOWN_100K = 0,
        PULLUP_47K = 1,
        PULLUP_100K = 2,
        PULLUP_22K = 3
    };

class TeensyFlexIO {
public: 
    /// FlexIO module selection
    enum FlexIOModule {
        FLEXIO1 = 0,
        FLEXIO2 = 1,
        FLEXIO3 = 2
    };

private:
    // Private member variables with leading underscore as per requirements
    FlexIOHandler* _flexio_handler;
    IMXRT_FLEXIO_t* _flexio;
    bool _is_initialized = false;
    

public:
    TeensyFlexIO(){};
    ~TeensyFlexIO();

    void begin(FlexIOModule module);
    bool isInitialized() { return _is_initialized; }

    // Get the FlexIO hardware pointer
    IMXRT_FLEXIO_t* getFlexIO() { return _flexio; }

    // Request a shifter from the FlexIO handler
    // If shifterNum is -1 (default), returns first available shifter
    // If specific shifter requested, returns that shifter if available or -1 if not
    int8_t requestShifter(int8_t shifterNum = -1);

    // Release a shifter back to the FlexIO handler
    void releaseShifter(uint8_t shifterIndex);

    uint8_t shiftersDMAChannel(uint8_t n);

    /**
     * @brief Configure a FlexIO shifter using a type-safe configuration struct
     * 
     * @param shifterIndex Index of the shifter to configure (0-7)
     * @param config Configuration struct containing all shifter settings
     * 
     * Start Bit behavior depends on ShifterMode:
     * - Transmit:
     *   0 - Start bit disabled, loads data on enable
     *   1 - Start bit disabled, loads data on first shift
     *   2 - Outputs start bit value 0 before loading data
     *   3 - Outputs start bit value 1 before loading data
     * 
     * - Receive/Match Store:
     *   0,1 - Start bit checking disabled
     *   2 - Sets error flag if start bit is not 0
     *   3 - Sets error flag if start bit is not 1
     * 
     * - State:
     *   Used to disable state outputs
     * 
     * - Logic:
     *   Used to mask logic pin inputs
     * 
     * Stop Bit behavior depends on ShifterMode:
     * - Transmit:
     *   0 - No stop bit, output pin disabled
     *   1 - 1 stop bit, output pin disabled
     *   2 - 0 stop bit, output pin enabled
     *   3 - 1 stop bit, output pin enabled
     * 
     * - Receive:
     *   0 - Continuous reception
     *   1 - 1 stop bit, disabled after stop
     *   2 - 2 stop bits, disabled after stop
     *   3 - 3 stop bits, disabled after stop
     * 
     * - Match Store:
     *   0 - Match on all data
     *   1 - Match until stop bit
     *   2 - Match until two stop bits
     *   3 - Match until three stop bits
     */
    void configureShifter(uint8_t shifterIndex, const ShifterConfig& config);

    void configureShifter(uint8_t shifterIndex, uint8_t mode = 0 , uint8_t pinSelect = 0, 
        uint8_t pinPolarity = 0, uint8_t pinConfig = 0, uint8_t inputSource = 0, 
        uint8_t timerSelect = 0, uint8_t timerPolarity = 0, uint8_t parallelWidth = 8, 
        uint8_t startBit = 0, uint8_t stopBit = 0);

    

    /**
     * @brief Read current configuration of a FlexIO shifter
     * @param shifterIndex Index of the shifter to read (0-7)
     * @return ShifterConfig struct containing current settings
     */
    ShifterConfig readShifterConfig(uint8_t shifterIndex);

    // Request a timer from the FlexIO handler
    int8_t requestTimer(int8_t timerNum = -1);

    // Release a timer back to the FlexIO handler
    void releaseTimer(uint8_t timerIndex);

    int8_t requestTimers(uint8_t cnt);

    void configureTimer(uint8_t timerIndex, uint8_t mode = 0, uint8_t pinSelect = 0, 
        uint8_t pinPolarity = 0, uint8_t pinConfig = 0, uint8_t triggerSource = 0, 
        int8_t triggerSelect = 0, uint8_t triggerPolarity = 0, uint8_t timerEnable = 0, 
        uint8_t timerDisable = 0, uint8_t timerReset = 0, uint8_t timerDecrement = 0, 
        uint8_t timerOutput = 0, uint8_t startBit = 0, uint8_t stopBit = 0, 
        uint8_t compHigh = 0, uint8_t compLow = 0);

    /**
     * @brief Configure a FlexIO timer using a type-safe configuration struct
     * 
     * @param timerIndex Index of the timer to configure (0-7)
     * @param config Configuration struct containing all timer settings
     * 
     * Timer Mode options:
     * - Disabled: Timer is disabled
     * - 8BitBaud: 8-bit baud/bit counter
     * - 8BitPWM: 8-bit PWM mode
     * - 16BitBaud: 16-bit baud/bit counter
     * - 16BitPWM: 16-bit PWM mode
     * - Dual8BitBaudBit: Dual 8-bit baud/bit counter
     * 
     * Timer Enable conditions:
     * - Always: Timer always enabled
     * - TimerTriggerHigh: Timer enabled on trigger high
     * - TriggerRisingEdge: Timer enabled on trigger rising edge
     * - TriggerBothEdges: Timer enabled on trigger both edges
     * 
     * Timer Disable conditions:
     * - Never: Timer never disabled
     * - TimerTriggerLow: Timer disabled on trigger low
     * - TriggerFallingEdge: Timer disabled on trigger falling edge
     * - TriggerBothEdges: Timer disabled on trigger both edges
     * 
     * Timer Reset conditions:
     * - Never: Timer never reset
     * - TimerTriggerLow: Timer reset on trigger low
     * - TriggerFallingEdge: Timer reset on trigger falling edge
     * - TriggerBothEdges: Timer reset on trigger both edges
     * 
     * Timer Decrement options:
     * - FlexIOClock: Decrement on FlexIO clock
     * - TriggerInput: Decrement on trigger input
     * - TriggerInputEdge: Decrement on trigger input edge
     * - PinInput: Decrement on pin input
     * 
     * Timer Output options:
     * - One: Output one when enabled and not affected by timer reset
     * - Zero: Output zero when enabled and not affected by timer reset
     * - OneResetZero: Output one when enabled and zero when reset
     * - ZeroResetOne: Output zero when enabled and one when reset
     */
    void configureTimer(uint8_t timerIndex, const TimerConfig& config);

    uint8_t calculateTriggerSelect(TriggerType type, uint8_t number);

    TimerConfig readTimerConfig(uint8_t timerIndex);

    bool setPinFlexioMode(uint8_t pin);

    bool setPinParameters(uint8_t pin, PullUp pullup = PullUp::DISABLED, uint8_t driveStrength = 0, 
        uint8_t slewRate = 2);

    // Enable FlexIO module operations
    void enable() { _flexio->CTRL |= FLEXIO_CTRL_FLEXEN; }
    
    // Disable FlexIO module operations
    void disable() { _flexio->CTRL &= ~FLEXIO_CTRL_FLEXEN; }

    // Basic data transfer methods
    void writeShifter(uint8_t shifterIndex, uint32_t data) { _flexio->SHIFTBUF[shifterIndex] = data; }
    uint32_t readShifter(uint8_t shifterIndex) { return _flexio->SHIFTBUFBIS[shifterIndex]; }
    
    // Status checking methods

    // Returns true if the shifter has data available to be read
    bool bufferReady(uint8_t shifterIndex) { return _flexio->SHIFTSTAT & (1 << shifterIndex); }

    // Clock control methods
    float setClock(float frequency);
    float setClockUsingAudioPLL(float frequency) { return _flexio_handler->setClockUsingAudioPLL(frequency); }
    float setClockUsingVideoPLL(float frequency) { return _flexio_handler->setClockUsingVideoPLL(frequency); }
    uint32_t getClockRate() { return _flexio_handler->computeClockRate(); }
    void setClockSettings(uint8_t clk_sel, uint8_t clk_pred, uint8_t clk_podf);

    void disableShifterInterrupt(uint8_t shifter);
    void enableShifterInterrupt(uint8_t shifter);

    void disableTimerInterrupt(uint8_t timer);
    void enableTimerInterrupt(uint8_t timer);

    void clearShifterStatus(uint8_t shifter);
    void clearTimerStatus(uint8_t timer);

    void clearShifterError(uint8_t shifter);


    // Get the FlexIO handler pointer (for advanced use)
    FlexIOHandler* getFlexIOHandler() { return _flexio_handler; }
};


#endif // _TEENSY_FLEX_IO_H_
