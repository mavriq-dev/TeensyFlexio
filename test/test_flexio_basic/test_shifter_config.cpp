#include <Arduino.h>
#include <unity.h>
#include "TeensyFlexIO.h"
#include "run_tests.h"

void test_basic_shifter_config(void) {
    // First request a shifter
    int8_t shifterIndex = flexIO1->requestShifter();
    TEST_ASSERT_GREATER_OR_EQUAL_INT8(0, shifterIndex);

    // set to default
    flexIO1->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 1, 0, 0 ); // 1 is needed to reset PWIDTH

    // Configure for basic input mode
    const uint8_t mode = 1;          // Receive mode
    const uint8_t pinIndex = 2;      // Pin 2
    const uint8_t pinPolarity = 0;   // Active high
    const uint8_t pinConfig = 2;     // Input
    const uint8_t inputSource = 0;   // Pin input
    const uint8_t timerIndex = 0;    // Timer 0
    const uint8_t timerPolarity = 0; // Positive edge
    const uint8_t parallelWidth = 7;         // 8-bit
    const uint8_t startBit = 0;      // Start at bit 0
    const uint8_t stopBit = 1;       // Stop at bit 7

    flexIO1->configureShifter(shifterIndex, mode, pinIndex, pinPolarity, pinConfig,
                            inputSource, timerIndex, timerPolarity, parallelWidth, startBit, stopBit);

    // Get the FlexIO hardware pointer
    IMXRT_FLEXIO_t* flexio = flexIO1->getFlexIO();

    // Verify SHIFTCTL register
    uint32_t expectedShiftctl = 0;
    expectedShiftctl |= (timerIndex & 0x7) << 24;    // TIMSEL
    expectedShiftctl |= (4 & 0x1F) << 8;     // PINSEL
    expectedShiftctl |= (pinPolarity & 0x1) << 7;    // PINPOL
    expectedShiftctl |= (pinConfig & 0x3) << 16;      // PINCONFIG
    expectedShiftctl |= (timerPolarity & 0x1) << 23; // TIMPOL
    expectedShiftctl |= (mode & 0x7) << 0;                  // SMOD

    TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexio->SHIFTCTL[shifterIndex]);

    // Verify SHIFTCFG register
    uint32_t expectedShiftcfg = 0;
    expectedShiftcfg |= ((parallelWidth ) & 0x1F) << 16;   // PWIDTH
    expectedShiftcfg |= (inputSource & 0x1) << 8;    // INSRC
    expectedShiftcfg |= (stopBit & 0x1F) << 4;       // SSTOP
    expectedShiftcfg |= (startBit & 0x1F) << 0;      // SSTART

    TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexio->SHIFTCFG[shifterIndex]);

    // Clean up
    flexIO1->releaseShifter(shifterIndex);
}

void test_spi_mode_shifter_config(void) {
    // Request a shifter for SPI MISO
    int8_t shifterIndex = flexIO1->requestShifter();
    TEST_ASSERT_GREATER_OR_EQUAL_INT8(0, shifterIndex);

    // set to default
    flexIO1->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 1, 0, 0 ); // 1 is needed to reset PWIDTH

    // Configure for SPI MISO (input) mode
    const uint8_t mode = 1;          // Receive mode
    const uint8_t pinIndex = 2;      // MISO pin
    const uint8_t pinPolarity = 0;   // Active high
    const uint8_t pinConfig = 2;     // Input
    const uint8_t inputSource = 0;   // Pin input
    const uint8_t timerIndex = 0;    // Timer 0
    const uint8_t timerPolarity = 1; // Sample on falling edge
    const uint8_t parallelWidth = 8;         // 8-bit
    const uint8_t startBit = 3;      // MSB first
    const uint8_t stopBit = 2;       // 8 bits

    flexIO1->configureShifter(shifterIndex, mode, pinIndex, pinPolarity, pinConfig,
                            inputSource, timerIndex, timerPolarity, parallelWidth, startBit, stopBit);

    IMXRT_FLEXIO_t* flexio = flexIO1->getFlexIO();

    // Verify SHIFTCTL configuration for SPI MISO
    uint32_t expectedShiftctl = 0;
    expectedShiftctl |= (timerIndex & 0x7) << 24;    // TIMSEL
    expectedShiftctl |= (4 & 0x1F) << 8;     // PINSEL
    expectedShiftctl |= (pinPolarity & 0x1) << 7;    // PINPOL
    expectedShiftctl |= (pinConfig & 0x3) << 16;      // PINCONFIG
    expectedShiftctl |= (timerPolarity & 0x1) << 23; // TIMPOL
    expectedShiftctl |= (mode & 0x7) << 0;                  // SMOD
    expectedShiftctl |= mode & 0x1;                  // SMOD

    TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexio->SHIFTCTL[shifterIndex]);

    // Clean up
    flexIO1->releaseShifter(shifterIndex);

}

void test_uart_mode_shifter_config(void) {
    // Request a shifter for UART RX
    int8_t shifterIndex = flexIO1->requestShifter();
    TEST_ASSERT_GREATER_OR_EQUAL_INT8(0, shifterIndex);

    // set to default
    flexIO1->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 1, 0, 0 ); // 1 is needed to reset PWIDTH

    // Configure for UART RX mode
    const uint8_t mode = 1;          // Receive mode
    const uint8_t pinIndex = 3;      // RX pin
    const uint8_t pinPolarity = 1;   // Active low (UART idle high)
    const uint8_t pinConfig = 2;     // Input
    const uint8_t inputSource = 0;   // Pin input
    const uint8_t timerIndex = 1;    // Timer 1
    const uint8_t timerPolarity = 0; // Sample on rising edge
    const uint8_t parallelWidth = 7;         // 8-bit
    const uint8_t startBit = 1;      // Start bit after idle
    const uint8_t stopBit = 1;       // Stop bit position (8 data + 1 stop)

    flexIO1->configureShifter(shifterIndex, mode, pinIndex, pinPolarity, pinConfig,
                            inputSource, timerIndex, timerPolarity, parallelWidth, startBit, stopBit);

    IMXRT_FLEXIO_t* flexio = flexIO1->getFlexIO();

    // Verify SHIFTCFG for UART mode
    uint32_t expectedShiftcfg = 0;
    expectedShiftcfg |= ((parallelWidth ) & 0x1F) << 16;   // PWIDTH
    expectedShiftcfg |= (inputSource & 0x1) << 8;    // INSRC
    expectedShiftcfg |= (stopBit & 0x1F) << 4;       // SSTOP
    expectedShiftcfg |= (startBit & 0x1F) << 0;      // SSTART

    TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexio->SHIFTCFG[shifterIndex]);

    // Clean up
    flexIO1->releaseShifter(shifterIndex);
}

// Test configuring shifter with maximum valid values for each field
void test_shifter_config_max_values(void) {
    uint8_t shifterIndex = 0;
    
    // Request the shifter first
    TEST_ASSERT_EQUAL_HEX8(0, flexIO1->requestShifter(shifterIndex));
    TEST_ASSERT_EQUAL_HEX8(0, flexIO2->requestShifter(shifterIndex));

    // set to default
    flexIO1->configureShifter(shifterIndex,0, 15, 0, 0, 0, 0, 0, 0, 0, 0 ); // 1 is needed to reset PWIDTH
    flexIO2->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    
    // Test mode maximum (7)
    flexIO1->configureShifter(
        shifterIndex,
        7,              // Mode (max 7)
        0, 0, 0, 0, 0, 0, 0, 0, 0
    );
    uint32_t expectedShiftctl = 7;    // SMOD = 7
    TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    
    // Test FlexIO1 pin select maximum (31)
    flexIO1->configureShifter(
        shifterIndex,
        0, 15, 0, 0, 0, 0, 0, 0, 0, 0
    );
    expectedShiftctl = (15 << 8);     // PINSEL = 31
    TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);

    // Test FlexIO2 pin select maximum (31)
    flexIO2->configureShifter(
        shifterIndex,
        0, 31, 0, 0, 0, 0, 0, 0, 0, 0
    );
    expectedShiftctl = (31 << 8);     // PINSEL = 31
    TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO2->getFlexIO()->SHIFTCTL[shifterIndex]);
    
    // Test pin polarity maximum (1)
    flexIO1->configureShifter(
        shifterIndex,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0
    );
    expectedShiftctl = (1 << 7);      // PINPOL = 1
    TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    
    // Test pin config maximum (3)
    flexIO1->configureShifter(
        shifterIndex,
        0, 0, 0, 3, 0, 0, 0, 0, 0, 0
    );
    expectedShiftctl = (3 << 16);     // PINCFG = 3
    TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    
    // Test FlexIO1 input source maximum (1)
    flexIO1->configureShifter(
        shifterIndex,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0
    );
    // When INSRC=1 (shifter N+1 output), PWIDTH is automatically set to maximum (0xF)
    // This is hardware behavior to support parallel transfer between shifters
    uint32_t expectedShiftcfg = (1 << 8);    // INSRC = 1, PWIDTH = 0xF
    TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);

    // Test FlexIO2 input source maximum (1)
    flexIO2->configureShifter(
        shifterIndex,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0
    );
    // When INSRC=1 (shifter N+1 output), PWIDTH is automatically set to maximum (0xF)
    // This is hardware behavior to support parallel transfer between shifters
    expectedShiftcfg = (1 << 8);    // INSRC = 1, PWIDTH = 0xF
    TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO2->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test timer select maximum (7)
    flexIO1->configureShifter(
        shifterIndex,
        0, 0, 0, 0, 0, 7, 0, 0, 0, 0
    );
    expectedShiftctl = (7 << 24);     // TIMSEL = 7
    TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    
    // Test timer polarity maximum (1)
    flexIO1->configureShifter(
        shifterIndex,
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0
    );
    expectedShiftctl = (1 << 23);     // TIMPOL = 1
    TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    
    // Test parallel width maximum (15)
    flexIO1->configureShifter(
        shifterIndex,
        0, 0, 0, 0, 0, 0, 0, 15, 0, 0
    );
    expectedShiftcfg = (15 << 16);    // PWIDTH = parallelWidth-1
    TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test FlexIO1 start bit maximum (3)
    flexIO1->configureShifter(
        shifterIndex,
        0, 0, 0, 0, 0, 0, 0, 0, 3, 0
    );
    // When start bit is configured, PWIDTH is automatically set to maximum (0xF)
    // This is hardware behavior to ensure proper start bit handling
    expectedShiftcfg = (3 << 0) ;    // SSTART = 3, PWIDTH = 0xF
    TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);

    // Test FlexIO2 start bit maximum (3)
    flexIO2->configureShifter(
        shifterIndex,
        0, 0, 0, 0, 0, 0, 0, 0, 3, 0
    );
    // When start bit is configured, PWIDTH is automatically set to maximum (0xF)
    // This is hardware behavior to ensure proper start bit handling
    expectedShiftcfg = (3 << 0);    // SSTART = 3, PWIDTH = 0xF
    TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO2->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test FlexIO1 stop bit maximum (3)
    flexIO1->configureShifter(
        shifterIndex,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 3
    );
    expectedShiftcfg = (3 << 4);      // SSTOP = 3
    TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);

    // Test FlexIO2 stop bit maximum (3)
    flexIO2->configureShifter(
        shifterIndex,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 3
    );
    expectedShiftcfg = (3 << 4);      // SSTOP = 3
    TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO2->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Clean up
    flexIO1->releaseShifter(shifterIndex);
    flexIO2->releaseShifter(shifterIndex);
}

// Test each field boundary individually
void test_shifter_config_field_boundaries(void) {
    uint8_t shifterIndex = 0;
    
    // Request the shifter first
    TEST_ASSERT_EQUAL_HEX8(0, flexIO1->requestShifter(shifterIndex));
    TEST_ASSERT_EQUAL_HEX8(0, flexIO2->requestShifter(shifterIndex));

    flexIO1->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 1, 0, 0 ); // 1 is needed to reset PWIDTH to default
    flexIO2->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 1, 0, 0 );
    
    // Save original register values
    uint32_t originalShiftctl = flexIO1->getFlexIO()->SHIFTCTL[shifterIndex];
    uint32_t originalShiftcfg = flexIO1->getFlexIO()->SHIFTCFG[shifterIndex];

    // Test shifter index (max 7)
    flexIO1->configureShifter(8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test mode boundary (max 7)
    flexIO1->configureShifter(shifterIndex, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test FlexIO1 pin select boundary (max 15)
    flexIO1->configureShifter(shifterIndex, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);

    // Test FlexIO2 pin select boundary (max 31)
    flexIO2->configureShifter(shifterIndex, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO2->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO2->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test pin polarity boundary (max 1)
    flexIO1->configureShifter(shifterIndex, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test pin config boundary (max 3)
    flexIO1->configureShifter(shifterIndex, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test input source boundary (max 1)
    flexIO1->configureShifter(shifterIndex, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test timer select boundary (max 7)
    flexIO1->configureShifter(shifterIndex, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test timer polarity boundary (max 1)
    flexIO1->configureShifter(shifterIndex, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test parallelWidth boundary (max 31)
    flexIO1->configureShifter(shifterIndex, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test start bit boundary (max 7)
    flexIO1->configureShifter(shifterIndex, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Test stop bit boundary (max 7)
    flexIO1->configureShifter(shifterIndex, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8);
    TEST_ASSERT_EQUAL_HEX32(originalShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    
    // Clean up
    flexIO1->releaseShifter(shifterIndex);
    flexIO2->releaseShifter(shifterIndex);
}



// Test configuring shifter with maximum valid values for each field
void test_shifter_config_struct_max_values(void) {
    uint8_t shifterIndex = 0;
    
    // Request the shifter first
    TEST_ASSERT_EQUAL_HEX8(0, flexIO1->requestShifter(shifterIndex));
    TEST_ASSERT_EQUAL_HEX8(0, flexIO2->requestShifter(shifterIndex));

    
    flexIO1->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 1, 0, 0 ); // 1 is needed to reset PWIDTH to default
    flexIO2->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 1, 0, 0 );

    // Set to default state
    ShifterConfig defaultConfig;
    flexIO1->configureShifter(shifterIndex, defaultConfig);
    flexIO2->configureShifter(shifterIndex, defaultConfig);
    
    // Test mode maximum
    {
        ShifterConfig config;
        config.mode = ShifterMode::Logic;  // Max value 7
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftctl = static_cast<uint8_t>(ShifterMode::Logic);
        TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    }
    
    // Test FlexIO1 pin select maximum
    {
        ShifterConfig config;
        config.pinSelect = 15;  // Max for FlexIO1
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftctl = (15 << 8);
        TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    }

    // Test FlexIO2 pin select maximum
    {
        ShifterConfig config;
        config.pinSelect = 31;  // Max for FlexIO2
        flexIO2->configureShifter(shifterIndex, config);
        uint32_t expectedShiftctl = (31 << 8);
        TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO2->getFlexIO()->SHIFTCTL[shifterIndex]);
    }
    
    // Test pin polarity maximum
    {
        ShifterConfig config;
        config.pinPolarity = PinPolarity::ActiveLow;
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftctl = (1 << 7);
        TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    }
    
    // Test pin config maximum
    {
        ShifterConfig config;
        config.pinConfig = PinConfig::Output;
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftctl = (3 << 16);
        TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    }
    
    // Test input source maximum
    {
        ShifterConfig config;
        config.inputSource = InputSource::Shifter;
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftcfg = (1 << 8) ;
        TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    }

    // Test input source maximum
    {
        ShifterConfig config;
        config.inputSource = InputSource::Shifter;
        flexIO2->configureShifter(shifterIndex, config);
        uint32_t expectedShiftcfg = (1 << 8);
        TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO2->getFlexIO()->SHIFTCFG[shifterIndex]);
    }


    // Test timer select maximum
    {
        ShifterConfig config;
        config.timerSelect = 7;  // Max value 7
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftctl = (7 << 24);
        TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    }

    // Test timer polarity maximum
    {
        ShifterConfig config;
        config.timerPolarity = TimerPolarity::ActiveLow;
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftctl = (1 << 23);
        TEST_ASSERT_EQUAL_HEX32(expectedShiftctl, flexIO1->getFlexIO()->SHIFTCTL[shifterIndex]);
    }

    // Test parallel width maximum
    {
        ShifterConfig config;
        config.parallelWidth = 15;  // Max value 15
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftcfg = (15 << 16);  // PWIDTH is parallelWidth-1
        TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    }

    // Test start bit maximum
    {
        ShifterConfig config;
        config.startBit = 3;  // Max value 3
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftcfg = 3;
        TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    }

    // Test stop bit maximum
    {
        ShifterConfig config;
        config.stopBit = 3;  // Max value 3
        flexIO1->configureShifter(shifterIndex, config);
        uint32_t expectedShiftcfg = (3 << 4);
        TEST_ASSERT_EQUAL_HEX32(expectedShiftcfg, flexIO1->getFlexIO()->SHIFTCFG[shifterIndex]);
    }

    flexIO1->releaseShifter(shifterIndex);
    flexIO2->releaseShifter(shifterIndex);
}

void test_read_shifter_config(void) {

    uint8_t shifterIndex = 0;
    TEST_ASSERT_EQUAL_HEX8(0, flexIO1->requestShifter(shifterIndex));
    TEST_ASSERT_EQUAL_HEX8(0, flexIO2->requestShifter(shifterIndex));

    flexIO1->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 1, 0, 0 ); // 1 is needed to reset PWIDTH to default
    flexIO2->configureShifter(shifterIndex,0, 0, 0, 0, 0, 0, 0, 1, 0, 0 );

      // Set to default state
    ShifterConfig defaultConfig;
    flexIO1->configureShifter(shifterIndex, defaultConfig);
    flexIO2->configureShifter(shifterIndex, defaultConfig);

    // Test reading default configuration
    {
        ShifterConfig config;
        flexIO1->configureShifter(shifterIndex, config);
        
        ShifterConfig readConfig = flexIO1->readShifterConfig(shifterIndex);
        
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.mode), static_cast<uint8_t>(readConfig.mode));
        TEST_ASSERT_EQUAL(config.pinSelect, readConfig.pinSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinPolarity), static_cast<uint8_t>(readConfig.pinPolarity));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinConfig), static_cast<uint8_t>(readConfig.pinConfig));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.inputSource), static_cast<uint8_t>(readConfig.inputSource));
        TEST_ASSERT_EQUAL(config.timerSelect, readConfig.timerSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerPolarity), static_cast<uint8_t>(readConfig.timerPolarity));
        TEST_ASSERT_EQUAL(config.parallelWidth, readConfig.parallelWidth);
        TEST_ASSERT_EQUAL(config.startBit, readConfig.startBit);
        TEST_ASSERT_EQUAL(config.stopBit, readConfig.stopBit);
    }


    // Test reading maximum values configuration
    {
        ShifterConfig config;
        config.mode = ShifterMode::Logic;
        config.pinSelect = 15;  // Max for FlexIO1
        config.pinPolarity = PinPolarity::ActiveLow;
        config.pinConfig = PinConfig::Output;
        config.inputSource = InputSource::Shifter;
        config.timerSelect = 7;
        config.timerPolarity = TimerPolarity::ActiveLow;
        config.parallelWidth = 15;
        config.startBit = 3;
        config.stopBit = 3;

        flexIO1->configureShifter(shifterIndex, config);

        ShifterConfig readConfig = flexIO1->readShifterConfig(shifterIndex);

        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.mode), static_cast<uint8_t>(readConfig.mode));
        TEST_ASSERT_EQUAL(config.pinSelect, readConfig.pinSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinPolarity), static_cast<uint8_t>(readConfig.pinPolarity));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinConfig), static_cast<uint8_t>(readConfig.pinConfig));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.inputSource), static_cast<uint8_t>(readConfig.inputSource));
        TEST_ASSERT_EQUAL(config.timerSelect, readConfig.timerSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerPolarity), static_cast<uint8_t>(readConfig.timerPolarity));
        TEST_ASSERT_EQUAL(config.parallelWidth, readConfig.parallelWidth);
        TEST_ASSERT_EQUAL(config.startBit, readConfig.startBit);
        TEST_ASSERT_EQUAL(config.stopBit, readConfig.stopBit);
    }

    // Test reading FlexIO2 maximum values configuration
    {
        ShifterConfig config;
        config.mode = ShifterMode::Logic;
        config.pinSelect = 31;  // Max for FlexIO2
        config.pinPolarity = PinPolarity::ActiveLow;
        config.pinConfig = PinConfig::Output;
        config.inputSource = InputSource::Shifter;
        config.timerSelect = 7;
        config.timerPolarity = TimerPolarity::ActiveLow;
        config.parallelWidth = 15;
        config.startBit = 3;
        config.stopBit = 3;

        flexIO2->configureShifter(shifterIndex, config);
        ShifterConfig readConfig = flexIO2->readShifterConfig(shifterIndex);
        
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.mode), static_cast<uint8_t>(readConfig.mode));
        TEST_ASSERT_EQUAL(config.pinSelect, readConfig.pinSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinPolarity), static_cast<uint8_t>(readConfig.pinPolarity));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.pinConfig), static_cast<uint8_t>(readConfig.pinConfig));
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.inputSource), static_cast<uint8_t>(readConfig.inputSource));
        TEST_ASSERT_EQUAL(config.timerSelect, readConfig.timerSelect);
        TEST_ASSERT_EQUAL(static_cast<uint8_t>(config.timerPolarity), static_cast<uint8_t>(readConfig.timerPolarity));
        TEST_ASSERT_EQUAL(config.parallelWidth, readConfig.parallelWidth);
        TEST_ASSERT_EQUAL(config.startBit, readConfig.startBit);
        TEST_ASSERT_EQUAL(config.stopBit, readConfig.stopBit);
    }

    flexIO1->releaseShifter(shifterIndex);
    flexIO2->releaseShifter(shifterIndex);
}
