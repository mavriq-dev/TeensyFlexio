#include <Arduino.h>
#include <unity.h>
#include "TeensyFlexIO.h"
#include "run_tests.h"

void test_basic_timer_config(void) {
    // First request a timer
    int8_t timerIndex = flexIO1->requestTimer();
    TEST_ASSERT_GREATER_OR_EQUAL_INT8(0, timerIndex);

    // Set to default state
    flexIO1->configureTimer(timerIndex, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    // Configure for basic output mode
    const uint8_t mode = 1;          // PWM mode
    const uint8_t pinIndex = 2;      // Pin 2
    const uint8_t pinPolarity = 0;   // Active high
    const uint8_t pinConfig = 1;     // Output
    const uint8_t triggerSource = 0; // Internal trigger
    const uint8_t triggerSelect = 0; // Timer trigger input
    const uint8_t triggerPolarity = 0; // Positive edge
    const uint8_t timerEnable = 1;   // Enable on trigger high
    const uint8_t timerDisable = 0;  // Disable on timer compare
    const uint8_t timerReset = 0;    // Reset on timer disable
    const uint8_t timerDecrement = 0;// Decrement on FlexIO clock
    const uint8_t timerOutput = 1;   // Output on enable and reset
    const uint8_t startBit = 0;      // Start at bit 0
    const uint8_t stopBit = 1;       // Stop at bit 7
    const uint8_t compHigh = 3;      // PWM high time
    const uint8_t compLow = 6;       // PWM period

    flexIO1->configureTimer(timerIndex, mode, pinIndex, pinPolarity, pinConfig,
                          triggerSource, triggerSelect, triggerPolarity,
                          timerEnable, timerDisable, timerReset,
                          timerDecrement, timerOutput, startBit, stopBit,
                          compHigh, compLow);

    // Get the FlexIO hardware pointer
    IMXRT_FLEXIO_t* flexio = flexIO1->getFlexIO();

    // Verify TIMCTL register
    uint32_t expectedTimctl = 0;
    expectedTimctl |= (triggerSelect & 0x3F) << 24;  // TRGSEL
    expectedTimctl |= (triggerPolarity & 0x1) << 23; // TRGPOL
    expectedTimctl |= (triggerSource & 0x1) << 22;   // TRGSRC
    expectedTimctl |= (pinConfig & 0x3) << 16;       // PINCFG
    expectedTimctl |= (4 & 0x1F) << 8;        // PINSEL
    expectedTimctl |= (pinPolarity & 0x1) << 7;      // PINPOL
    expectedTimctl |= (mode & 0x7);                  // TIMOD

    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexio->TIMCTL[timerIndex]);

    // Verify TIMCFG register
    uint32_t expectedTimcfg = 0;
    expectedTimcfg |= (timerOutput & 0x3) << 24;     // TIMOUT
    expectedTimcfg |= (timerDecrement & 0x3) << 20;  // TIMDEC
    expectedTimcfg |= (timerReset & 0x7) << 16;      // TIMRST
    expectedTimcfg |= (timerDisable & 0x7) << 12;    // TIMDIS
    expectedTimcfg |= (timerEnable & 0x7) << 8;      // TIMENA
    expectedTimcfg |= (stopBit & 0x7) << 4;          // TSTOP
    expectedTimcfg |= (startBit & 0x1) << 1;         // TSTART

    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexio->TIMCFG[timerIndex]);

    // Verify TIMCMP register
    uint32_t expectedTimcmp = (compHigh << 8) | compLow;
    TEST_ASSERT_EQUAL_HEX32(expectedTimcmp, flexio->TIMCMP[timerIndex]);

    // Clean up
    flexIO1->releaseTimer(timerIndex);
}

void test_spi_mode_timer_config(void) {
    // Request a timer for SPI clock
    int8_t timerIndex = flexIO1->requestTimer();
    TEST_ASSERT_GREATER_OR_EQUAL_INT8(0, timerIndex);

    // Set to default state
    flexIO1->configureTimer(timerIndex, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    // Configure for SPI clock generation
    const uint8_t mode = 3;          // Dual 8-bit counters baud/bit
    const uint8_t pinIndex = 2;      // SCK pin
    const uint8_t pinPolarity = 0;   // Active high
    const uint8_t pinConfig = 1;     // Output
    const uint8_t triggerSource = 0; // Internal trigger
    const uint8_t triggerSelect = 0; // Timer trigger input
    const uint8_t triggerPolarity = 0; // Positive edge
    const uint8_t timerEnable = 1;   // Enable on trigger high
    const uint8_t timerDisable = 2;  // Disable on compare
    const uint8_t timerReset = 0;    // Reset on timer disable
    const uint8_t timerDecrement = 0;// Decrement on FlexIO clock
    const uint8_t timerOutput = 2;   // Output on timer enable and compare
    const uint8_t startBit = 0;      // Start at bit 0
    const uint8_t stopBit = 0;       // Single bit
    const uint8_t compHigh = 7;      // Baud counter
    const uint8_t compLow = 7;       // Bit counter

    flexIO1->configureTimer(timerIndex, mode, pinIndex, pinPolarity, pinConfig,
                          triggerSource, triggerSelect, triggerPolarity,
                          timerEnable, timerDisable, timerReset,
                          timerDecrement, timerOutput, startBit, stopBit,
                          compHigh, compLow);

    IMXRT_FLEXIO_t* flexio = flexIO1->getFlexIO();

    // Verify TIMCTL register
    uint32_t expectedTimctl = 0;
    expectedTimctl |= (triggerSelect & 0x3F) << 24;  // TRGSEL
    expectedTimctl |= (triggerPolarity & 0x1) << 23; // TRGPOL
    expectedTimctl |= (triggerSource & 0x1) << 22;   // TRGSRC
    expectedTimctl |= (pinConfig & 0x3) << 16;       // PINCFG
    expectedTimctl |= (4 & 0x1F) << 8;        // PINSEL
    expectedTimctl |= (pinPolarity & 0x1) << 7;      // PINPOL
    expectedTimctl |= (mode & 0x7);                  // TIMOD

    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexio->TIMCTL[timerIndex]);

    // Verify TIMCFG register
    uint32_t expectedTimcfg = 0;
    expectedTimcfg |= (timerOutput & 0x3) << 24;     // TIMOUT
    expectedTimcfg |= (timerDecrement & 0x3) << 20;  // TIMDEC
    expectedTimcfg |= (timerReset & 0x7) << 16;      // TIMRST
    expectedTimcfg |= (timerDisable & 0x7) << 12;    // TIMDIS
    expectedTimcfg |= (timerEnable & 0x7) << 8;      // TIMENA
    expectedTimcfg |= (stopBit & 0x7) << 4;          // TSTOP
    expectedTimcfg |= (startBit & 0x1) << 1;         // TSTART

    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexio->TIMCFG[timerIndex]);

    // Verify TIMCMP register
    uint32_t expectedTimcmp = (compHigh << 8) | compLow;
    TEST_ASSERT_EQUAL_HEX32(expectedTimcmp, flexio->TIMCMP[timerIndex]);

    // Clean up
    flexIO1->releaseTimer(timerIndex);
}

void test_uart_mode_timer_config(void) {
    // Request a timer for UART baud rate generation
    int8_t timerIndex = flexIO1->requestTimer();
    TEST_ASSERT_GREATER_OR_EQUAL_INT8(0, timerIndex);

    // Set to default state
    flexIO1->configureTimer(timerIndex, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    // Configure for UART baud rate generation
    const uint8_t mode = 3;          // Dual 8-bit counters baud/bit
    const uint8_t pinIndex = 3;      // TX pin
    const uint8_t pinPolarity = 1;   // Active low (UART idle high)
    const uint8_t pinConfig = 1;     // Output
    const uint8_t triggerSource = 0; // Internal trigger
    const uint8_t triggerSelect = 0; // Timer trigger input
    const uint8_t triggerPolarity = 0; // Positive edge
    const uint8_t timerEnable = 1;   // Enable on trigger high
    const uint8_t timerDisable = 2;  // Disable on timer compare
    const uint8_t timerReset = 0;    // Reset on timer disable
    const uint8_t timerDecrement = 0;// Decrement on FlexIO clock
    const uint8_t timerOutput = 2;   // Output on timer enable and compare
    const uint8_t startBit = 0;      // Start at bit 0
    const uint8_t stopBit = 1;       // 8 bits + start + stop
    const uint8_t compHigh = 7;      // Baud counter
    const uint8_t compLow = 7;       // Bit counter

    flexIO1->configureTimer(timerIndex, mode, pinIndex, pinPolarity, pinConfig,
                          triggerSource, triggerSelect, triggerPolarity,
                          timerEnable, timerDisable, timerReset,
                          timerDecrement, timerOutput, startBit, stopBit,
                          compHigh, compLow);

    IMXRT_FLEXIO_t* flexio = flexIO1->getFlexIO();

    // Verify TIMCTL register
    uint32_t expectedTimctl = 0;
    expectedTimctl |= (triggerSelect & 0x3F) << 24;  // TRGSEL
    expectedTimctl |= (triggerPolarity & 0x1) << 23; // TRGPOL
    expectedTimctl |= (triggerSource & 0x1) << 22;   // TRGSRC
    expectedTimctl |= (pinConfig & 0x3) << 16;       // PINCFG
    expectedTimctl |= (5 & 0x1F) << 8;        // PINSEL
    expectedTimctl |= (pinPolarity & 0x1) << 7;      // PINPOL
    expectedTimctl |= (mode & 0x7);                  // TIMOD

    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexio->TIMCTL[timerIndex]);

    // Verify TIMCFG register
    uint32_t expectedTimcfg = 0;
    expectedTimcfg |= (timerOutput & 0x3) << 24;     // TIMOUT
    expectedTimcfg |= (timerDecrement & 0x3) << 20;  // TIMDEC
    expectedTimcfg |= (timerReset & 0x7) << 16;      // TIMRST
    expectedTimcfg |= (timerDisable & 0x7) << 12;    // TIMDIS
    expectedTimcfg |= (timerEnable & 0x7) << 8;      // TIMENA
    expectedTimcfg |= (stopBit & 0x7) << 4;          // TSTOP
    expectedTimcfg |= (startBit & 0x1) << 1;         // TSTART

    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexio->TIMCFG[timerIndex]);

    // Verify TIMCMP register
    uint32_t expectedTimcmp = (compHigh << 8) | compLow;
    TEST_ASSERT_EQUAL_HEX32(expectedTimcmp, flexio->TIMCMP[timerIndex]);

    // Clean up
    flexIO1->releaseTimer(timerIndex);
}

// Test configuring timer with maximum valid values for each field
void test_timer_config_max_values(void) {
    int8_t timerIndex1;
    int8_t timerIndex2;

    // Request the timer first
    TEST_ASSERT_EQUAL_HEX8(0, timerIndex1 = flexIO1->requestTimer());
    TEST_ASSERT_EQUAL_HEX8(0, timerIndex2 = flexIO2->requestTimer());

    // Set to default state
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    flexIO2->configureTimer(timerIndex2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Test mode maximum (3)
    flexIO1->configureTimer(
        timerIndex1,
        3,              // Mode (max 3)
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0            // Max compare values
    );
    uint32_t expectedTimctl = 3;    // TIMOD = 3
    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);
    
    // Test FlexIO1 pin select maximum (15)
    flexIO1->configureTimer(
        timerIndex1,
        0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimctl = (15 << 8);     // PINSEL = 15
    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test FlexIO2 pin select maximum (31)
    flexIO2->configureTimer(
        timerIndex2,
        0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimctl = (31 << 8);     // PINSEL = 31
    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO2->getFlexIO()->TIMCTL[timerIndex2]);
    
    // Test pin polarity maximum (1)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimctl = (1 << 7);      // PINPOL = 1
    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);
    
    // Test pin config maximum (3)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimctl = (3 << 16);     // PINCFG = 3
    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);
    
    // Test trigger source maximum (1)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimctl = (1 << 22);     // TRGSRC = 1
    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test trigger select maximum (63)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimctl = (31 << 24);    // TRGSEL = 63
    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test trigger polarity maximum (1)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimctl = (1 << 23);     // TRGPOL = 1
    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test timer enable maximum (7)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    uint32_t expectedTimcfg = (7 << 8);    // TIMENA = 7
    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test timer disable maximum (6)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimcfg = (6 << 12);    // TIMDIS = 6
    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test timer reset maximum (7)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimcfg = (7 << 16);    // TIMRST = 7
    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test timer decrement maximum (3)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimcfg = (3 << 20);    // TIMDEC = 3
    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test timer output maximum (3)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
        7, 7            // Max compare values
    );
    expectedTimcfg = (3 << 24);    // TIMOUT = 3
    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test start bit maximum (1)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        7, 7            // Max compare values
    );
    expectedTimcfg = (1 << 1);    // TSTART = 1
    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test stop bit maximum (3)
    flexIO1->configureTimer(
        timerIndex1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        7, 7            // Max compare values
    );
    expectedTimcfg = (3 << 4);    // TSTOP = 3
    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test compare values maximum
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF);
    uint32_t expectedTimcmp = (0xFF << 8) | 0xFF;
    TEST_ASSERT_EQUAL_HEX32(expectedTimcmp, flexIO1->getFlexIO()->TIMCMP[timerIndex1]);

    flexIO1->releaseTimer(timerIndex1);
    flexIO2->releaseTimer(timerIndex2);
}

void test_timer_config_struct_max_values(void) {
    uint8_t timerIndex1;
    uint8_t timerIndex2;
    
    // Request the timer first
    TEST_ASSERT_EQUAL_HEX8(0, timerIndex1 = flexIO1->requestTimer());
    TEST_ASSERT_EQUAL_HEX8(0, timerIndex2 = flexIO2->requestTimer());

    // Set to default state
    TimerConfig defaultConfig;
    flexIO1->configureTimer(timerIndex1, defaultConfig);
    flexIO2->configureTimer(timerIndex2, defaultConfig);
    
    // Test FlexIO1 pin select maximum (15)
    {
        TimerConfig config;
        config.pinSelect = 15;  // Max for FlexIO1
        flexIO1->configureTimer(timerIndex1, config);
        uint32_t expectedTimctl = (15 << 8);
        TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);
    }

    // Test FlexIO2 pin select maximum (31)
    {
        TimerConfig config;
        config.pinSelect = 31;  // Max for FlexIO2
        flexIO2->configureTimer(timerIndex2, config);
        uint32_t expectedTimctl = (31 << 8);
        TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO2->getFlexIO()->TIMCTL[timerIndex2]);
    }

    // Test trigger select maximum
    {
        TimerConfig config;
        config.triggerSelect = 31;  // Max value based on documentation
        flexIO1->configureTimer(timerIndex1, config);
        uint32_t expectedTimctl = (31 << 24);
        TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);
    }

    // Test start bit maximum
    {
        TimerConfig config;
        config.startBit = 1;  // Max value 1
        flexIO1->configureTimer(timerIndex1, config);
        uint32_t expectedTimcfg = (1 << 1);
        TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);
    }

    // Test stop bit maximum
    {
        TimerConfig config;
        config.stopBit = 3;  // Max value 3
        flexIO1->configureTimer(timerIndex1, config);
        uint32_t expectedTimcfg = (3 << 4);
        TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);
    }

    // Test compare values in SingleCounter mode
    {
        TimerConfig config;
        config.mode = TimerMode::SingleCounter;
        config.asCounter().compareValue = 5;
        config.asCounter().reloadValue = 3;
        flexIO1->configureTimer(timerIndex1, config);
        uint32_t expectedTimcmp = (5 << 8) | 3;
        TEST_ASSERT_EQUAL_HEX32(expectedTimcmp, flexIO1->getFlexIO()->TIMCMP[timerIndex1]);
    }

    // Test compare values in DualCounter mode
    {
        TimerConfig config;
        config.mode = TimerMode::Baud;
        config.asDual().bits_in_word = 7;
        config.asDual().baud_rate_div = 2;
        flexIO1->configureTimer(timerIndex1, config);
        uint32_t expectedTimcmp = (7 << 8) | 2;
        TEST_ASSERT_EQUAL_HEX32(expectedTimcmp, flexIO1->getFlexIO()->TIMCMP[timerIndex1]);
    }

    // Test compare values in DualPWM mode
    {
        TimerConfig config;
        config.mode = TimerMode::PWM;
        config.asPWM().highPeriod = 3;
        config.asPWM().lowPeriod = 6;
        flexIO1->configureTimer(timerIndex1, config);
        uint32_t expectedTimcmp = (3 << 8) | 6;
        TEST_ASSERT_EQUAL_HEX32(expectedTimcmp, flexIO1->getFlexIO()->TIMCMP[timerIndex1]);
    }

    flexIO1->releaseTimer(timerIndex1);
    flexIO2->releaseTimer(timerIndex2);
}

// Test each field boundary individually
void test_timer_config_field_boundaries(void) {
    uint8_t timerIndex1;
    uint8_t timerIndex2;
    
    // Request the timer first
    TEST_ASSERT_EQUAL_HEX8(0, timerIndex1 = flexIO1->requestTimer());
    TEST_ASSERT_EQUAL_HEX8(0, timerIndex2 = flexIO2->requestTimer());

    // Set to default state
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    flexIO2->configureTimer(timerIndex2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Save original register values
    uint32_t originalTimctl1 = flexIO1->getFlexIO()->TIMCTL[timerIndex1];
    uint32_t originalTimcfg1 = flexIO1->getFlexIO()->TIMCFG[timerIndex1];
    uint32_t originalTimcmp1 = flexIO1->getFlexIO()->TIMCMP[timerIndex1];
    uint32_t originalTimctl2 = flexIO2->getFlexIO()->TIMCTL[timerIndex2];
    // uint32_t originalTimcfg2 = flexIO2->getFlexIO()->TIMCFG[timerIndex2];
    // uint32_t originalTimcmp2 = flexIO2->getFlexIO()->TIMCMP[timerIndex2];

    // Test invalid timer index (8)
    flexIO1->configureTimer(8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimctl1, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test mode boundary (max 3)
    flexIO1->configureTimer(timerIndex1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimctl1, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test FlexIO1 pin select boundary (max 15)
    flexIO1->configureTimer(timerIndex1, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimctl1, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test FlexIO2 pin select boundary (max 31)
    flexIO2->configureTimer(timerIndex2, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimctl2, flexIO2->getFlexIO()->TIMCTL[timerIndex2]);

    // Test pin polarity boundary (max 1)
    flexIO1->configureTimer(timerIndex1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimctl1, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test pin config boundary (max 3)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimctl1, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test trigger source boundary (max 1)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimctl1, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test trigger select boundary (max 63)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimctl1, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test trigger polarity boundary (max 1)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimctl1, flexIO1->getFlexIO()->TIMCTL[timerIndex1]);

    // Test timer enable boundary (max 7)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg1, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test timer disable boundary (max 7)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg1, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test timer reset boundary (max 7)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg1, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test timer decrement boundary (max 3)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg1, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test timer output boundary (max 3)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg1, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test start bit boundary (max 1)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg1, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test stop bit boundary (max 3)
    flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg1, flexIO1->getFlexIO()->TIMCFG[timerIndex1]);

    // Test compare high boundary (max 0xFF)
    // flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0);
    // TEST_ASSERT_EQUAL_HEX32(originalTimcmp1, flexIO1->getFlexIO()->TIMCMP[timerIndex1]);

    // Test compare low boundary (max 0xFF)
    // flexIO1->configureTimer(timerIndex1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF);
    // TEST_ASSERT_EQUAL_HEX32(originalTimcmp1, flexIO1->getFlexIO()->TIMCMP[timerIndex1]);

    // Test PWM mode specific constraints (high time must be <= period)
    flexIO1->configureTimer(timerIndex1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 4);
    TEST_ASSERT_EQUAL_HEX32(originalTimcmp1, flexIO1->getFlexIO()->TIMCMP[timerIndex1]);

    flexIO1->releaseTimer(timerIndex1);
    flexIO2->releaseTimer(timerIndex2);
}

void test_read_timer_config(void) {
    uint8_t timerIndex1;
    uint8_t timerIndex2;
    TEST_ASSERT_EQUAL_HEX8(0,timerIndex1 = flexIO1->requestTimer());
    TEST_ASSERT_EQUAL_HEX8(0,timerIndex2 = flexIO2->requestTimer());

    // Test reading default configuration
    {
        TimerConfig config;
        flexIO1->configureTimer(timerIndex1, config);
        TimerConfig readConfig = flexIO1->readTimerConfig(timerIndex1);
        
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.mode), static_cast<uint8_t>(readConfig.mode));
        TEST_ASSERT_EQUAL(config.pinSelect, readConfig.pinSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinPolarity), static_cast<uint8_t>(readConfig.pinPolarity));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinConfig), static_cast<uint8_t>(readConfig.pinConfig));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.triggerSource), static_cast<uint8_t>(readConfig.triggerSource));
        TEST_ASSERT_EQUAL(config.triggerSelect, readConfig.triggerSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.triggerPolarity), static_cast<uint8_t>(readConfig.triggerPolarity));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerEnable), static_cast<uint8_t>(readConfig.timerEnable));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerDisable), static_cast<uint8_t>(readConfig.timerDisable));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerReset), static_cast<uint8_t>(readConfig.timerReset));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerDecrement), static_cast<uint8_t>(readConfig.timerDecrement));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerOutput), static_cast<uint8_t>(readConfig.timerOutput));
        TEST_ASSERT_EQUAL(config.startBit, readConfig.startBit);
        TEST_ASSERT_EQUAL(config.stopBit, readConfig.stopBit);
        TEST_ASSERT_EQUAL(config.compHigh, readConfig.compHigh);
        TEST_ASSERT_EQUAL(config.compLow, readConfig.compLow);
    }

    // Test reading SingleCounter mode configuration
    {
        TimerConfig config;
        config.mode = TimerMode::SingleCounter;
        config.counter.compareValue = 5;
        config.counter.reloadValue = 3;
        
        flexIO1->configureTimer(timerIndex1, config);
        TimerConfig readConfig = flexIO1->readTimerConfig(timerIndex1);
        
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.mode), static_cast<uint8_t>(readConfig.mode));
        TEST_ASSERT_EQUAL(config.counter.compareValue, readConfig.counter.compareValue);
        TEST_ASSERT_EQUAL(config.counter.reloadValue, readConfig.counter.reloadValue);
    }

    // Test reading DualCounter mode configuration
    {
        TimerConfig config;
        config.mode = TimerMode::Baud;
        config.dual.bits_in_word = 7;
        config.dual.baud_rate_div = 2;
        
        flexIO1->configureTimer(timerIndex1, config);
        TimerConfig readConfig = flexIO1->readTimerConfig(timerIndex1);
        
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.mode), static_cast<uint8_t>(readConfig.mode));
        TEST_ASSERT_EQUAL(config.dual.baud_rate_div, readConfig.dual.baud_rate_div);
        TEST_ASSERT_EQUAL(config.dual.bits_in_word, readConfig.dual.bits_in_word);
    }

    // Test reading DualPWM mode configuration
    {
        TimerConfig config;
        config.mode = TimerMode::PWM;
        config.pwm.highPeriod = 3;
        config.pwm.lowPeriod = 6;
        
        flexIO1->configureTimer(timerIndex1, config);
        TimerConfig readConfig = flexIO1->readTimerConfig(timerIndex1);
        
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.mode), static_cast<uint8_t>(readConfig.mode));
        TEST_ASSERT_EQUAL(config.pwm.highPeriod, readConfig.pwm.highPeriod);
        TEST_ASSERT_EQUAL(config.pwm.lowPeriod, readConfig.pwm.lowPeriod);
    }

    // Test reading maximum values configuration for FlexIO1
    {
        TimerConfig config;
        config.mode = TimerMode::SingleCounter;
        config.pinSelect = 15;  // Max for FlexIO1
        config.pinPolarity = PinPolarity::ActiveLow;
        config.pinConfig = PinConfig::Output;
        config.triggerSource = TriggerSource::Internal;
        config.triggerSelect = 31;
        config.triggerPolarity = TriggerPolarity::ActiveLow;
        config.timerEnable = TimerEnable::TriggerRisingOrFalling;
        config.timerDisable = TimerDisable::TriggerFallingEdge;
        config.timerReset = TimerReset::OnDisable;
        config.timerDecrement = TimerDecrement::TriggerAndPin;
        config.timerOutput = TimerOutput::InvertedEnableAndReset;
        config.startBit = 1;
        config.stopBit = 3;
        config.counter.compareValue = 7;  // Max compare value
        config.counter.reloadValue = 7;   // Max reload value

        flexIO1->configureTimer(timerIndex1, config);
        TimerConfig readConfig = flexIO1->readTimerConfig(timerIndex1);
        
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.mode), static_cast<uint8_t>(readConfig.mode));
        TEST_ASSERT_EQUAL(config.pinSelect, readConfig.pinSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinPolarity), static_cast<uint8_t>(readConfig.pinPolarity));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinConfig), static_cast<uint8_t>(readConfig.pinConfig));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.triggerSource), static_cast<uint8_t>(readConfig.triggerSource));
        TEST_ASSERT_EQUAL(config.triggerSelect, readConfig.triggerSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.triggerPolarity), static_cast<uint8_t>(readConfig.triggerPolarity));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerEnable), static_cast<uint8_t>(readConfig.timerEnable));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerDisable), static_cast<uint8_t>(readConfig.timerDisable));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerReset), static_cast<uint8_t>(readConfig.timerReset));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerDecrement), static_cast<uint8_t>(readConfig.timerDecrement));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerOutput), static_cast<uint8_t>(readConfig.timerOutput));
        TEST_ASSERT_EQUAL(config.startBit, readConfig.startBit);
        TEST_ASSERT_EQUAL(config.stopBit, readConfig.stopBit);
        TEST_ASSERT_EQUAL(config.counter.compareValue, readConfig.counter.compareValue);
        TEST_ASSERT_EQUAL(config.counter.reloadValue, readConfig.counter.reloadValue);
    }

    // Test reading maximum values configuration for FlexIO2
    {
        TimerConfig config;
        config.mode = TimerMode::PWM;
        config.pinSelect = 31;  // Max for FlexIO2
        config.pinPolarity = PinPolarity::ActiveLow;
        config.pinConfig = PinConfig::Output;
        config.triggerSource = TriggerSource::Internal;
        config.triggerSelect = 31;
        config.triggerPolarity = TriggerPolarity::ActiveLow;
        config.timerEnable = TimerEnable::TriggerRisingOrFalling;
        config.timerDisable = TimerDisable::TriggerFallingEdge;
        config.timerReset = TimerReset::OnDisable;
        config.timerDecrement = TimerDecrement::TriggerAndPin;
        config.timerOutput = TimerOutput::InvertedEnableAndReset;
        config.startBit = 1;
        config.stopBit = 3;
        config.pwm.highPeriod = 3;  // Must be less than period
        config.pwm.lowPeriod = 7;    // Max period

        flexIO2->configureTimer(timerIndex2, config);
        TimerConfig readConfig = flexIO2->readTimerConfig(timerIndex2);
        
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.mode), static_cast<uint8_t>(readConfig.mode));
        TEST_ASSERT_EQUAL(config.pinSelect, readConfig.pinSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinPolarity), static_cast<uint8_t>(readConfig.pinPolarity));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinConfig), static_cast<uint8_t>(readConfig.pinConfig));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.triggerSource), static_cast<uint8_t>(readConfig.triggerSource));
        TEST_ASSERT_EQUAL(config.triggerSelect, readConfig.triggerSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.triggerPolarity), static_cast<uint8_t>(readConfig.triggerPolarity));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerEnable), static_cast<uint8_t>(readConfig.timerEnable));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerDisable), static_cast<uint8_t>(readConfig.timerDisable));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerReset), static_cast<uint8_t>(readConfig.timerReset));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerDecrement), static_cast<uint8_t>(readConfig.timerDecrement));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerOutput), static_cast<uint8_t>(readConfig.timerOutput));
        TEST_ASSERT_EQUAL(config.startBit, readConfig.startBit);
        TEST_ASSERT_EQUAL(config.stopBit, readConfig.stopBit);
        TEST_ASSERT_EQUAL(config.pwm.highPeriod, readConfig.pwm.highPeriod);
        TEST_ASSERT_EQUAL(config.pwm.lowPeriod, readConfig.pwm.lowPeriod);
    }

    flexIO1->releaseTimer(timerIndex1);
    flexIO2->releaseTimer(timerIndex2);
}
