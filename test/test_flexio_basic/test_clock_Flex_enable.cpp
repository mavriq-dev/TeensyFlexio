#include "run_tests.h"
#include "TeensyflexIO.h"
#include <unity.h>

// Test basic timer configuration and enable/disable
void test_timer_basic_enable_disable() {
    // TeensyflexIO1 flexIO1(TeensyflexIO1::flexIO11);
    
    // Request a timer
    int8_t timer = flexIO1->requestTimer();
    TEST_ASSERT_NOT_EQUAL(0xFF, timer);
    
    // Configure timer for basic PWM
    TimerConfig config;
    config.mode = TimerMode::Baud;  // Simple PWM mode
    config.pinSelect = 2;                       // Use pin 2
    config.pinConfig = PinConfig::Output;       // Pin is output
    config.timerOutput = TimerOutput::One;  // Output 1 when enabled, 0 when reset
    
    flexIO1->configureTimer(timer, config);
    
    // Set clock to 1MHz
    flexIO1->setClock(1000000);
    
    // Enable flexIO1
    flexIO1->enable();
    
    // Verify CTRL register has enable bit set
    TEST_ASSERT(flexIO1->getFlexIO()->CTRL & FLEXIO_CTRL_FLEXEN);
    
    // Disable flexIO1
    flexIO1->disable();
    
    // Verify CTRL register has enable bit cleared
    TEST_ASSERT_EQUAL(0, flexIO1->getFlexIO()->CTRL & FLEXIO_CTRL_FLEXEN);
    
    // Release timer
    flexIO1->releaseTimer(timer);
}

// Test timer with different clock settings
void test_timer_clock_settings() {
    // TeensyflexIO1 flexIO1(TeensyflexIO1::flexIO11);
    
    // Request a timer
    int8_t timer = flexIO1->requestTimer();
    TEST_ASSERT_NOT_EQUAL(0xFF, timer);
    
    // Try different clock sources
    float actual_freq = flexIO1->setClock(1000000);  // Request 1MHz
    uint32_t rate1 = flexIO1->getClockRate();
    TEST_ASSERT_FLOAT_WITHIN(actual_freq * 0.1, actual_freq, rate1);  // Within 10% of achieved frequency
    
    float freq2 = flexIO1->setClockUsingAudioPLL(48000);  // 48kHz
    uint32_t rate2 = flexIO1->getClockRate();
    TEST_ASSERT_FLOAT_WITHIN(freq2 * 0.1, freq2, rate2);  // Within 10% of achieved frequency
    
    // Release timer
    flexIO1->releaseTimer(timer);
}
