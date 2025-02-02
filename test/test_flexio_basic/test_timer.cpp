#include <Arduino.h>
#include <unity.h>
#include "TeensyFlexIO.h"
#include "run_tests.h"

void test_request_default_timer(void) {
    // Request a timer with default parameters
    uint8_t timerIndex = flexIO1->requestTimer();
    TEST_ASSERT_GREATER_OR_EQUAL_UINT8(0, timerIndex);
    TEST_ASSERT_LESS_THAN_UINT8(FlexIOHandler::CNT_TIMERS, timerIndex);
    
    // Clean up
    flexIO1->releaseTimer(timerIndex);
}

void test_request_specific_timer(void) {
    // Request a specific timer (timer 0)
    uint8_t timerIndex = flexIO1->requestTimer(0);
    TEST_ASSERT_EQUAL_HEX8(0, timerIndex);
    
    // Clean up
    flexIO1->releaseTimer(timerIndex);
}

void test_request_unavailable_timer(void) {
    // First request timer 0
    uint8_t firstTimer = flexIO1->requestTimer(0);
    TEST_ASSERT_EQUAL_HEX8(0, firstTimer);
    
    // Try to request the same timer again
    uint8_t secondTimer = flexIO1->requestTimer(0);
    TEST_ASSERT_EQUAL_INT8(-1, secondTimer);  // Should fail
    
    // Clean up
    flexIO1->releaseTimer(firstTimer);
}

void test_release_and_reclaim_timer(void) {
    // Request a specific timer
    uint8_t timerIndex = flexIO1->requestTimer(0);
    TEST_ASSERT_EQUAL_HEX8(0, timerIndex);
    
    // Release it
    flexIO1->releaseTimer(timerIndex);
    
    // Try to claim it again
    uint8_t reclaimedTimer = flexIO1->requestTimer(0);
    TEST_ASSERT_EQUAL_HEX8(0, reclaimedTimer);
    
    // Clean up
    flexIO1->releaseTimer(reclaimedTimer);
}

void test_multiple_timer_requests(void) {
    uint8_t timers[FlexIOHandler::CNT_TIMERS];
    
    // Request all available timers
    for(uint8_t i = 0; i < FlexIOHandler::CNT_TIMERS; i++) {
        timers[i] = flexIO1->requestTimer();
        TEST_ASSERT_GREATER_OR_EQUAL_UINT8(0, timers[i]);
        TEST_ASSERT_LESS_THAN_UINT8(FlexIOHandler::CNT_TIMERS, timers[i]);
    }
    
    // Try to request one more timer (should fail)
    uint8_t extraTimer = flexIO1->requestTimer();
    TEST_ASSERT_EQUAL_INT8(-1, extraTimer);
    
    // Release all timers
    for(uint8_t i = 0; i < FlexIOHandler::CNT_TIMERS; i++) {
        flexIO1->releaseTimer(timers[i]);
    }
}

// Test valid timer configuration
void test_timer_config_valid(void) {
    uint8_t timerIndex = 0;
    
    // Request the timer first
    TEST_ASSERT_EQUAL_HEX8(0, flexIO1->requestTimer(timerIndex));
    
    // Configure timer for basic PWM output
    flexIO1->configureTimer(
        timerIndex,      // Timer 0
        1,              // Mode 1 (PWM)
        0,              // Pin 0
        0,              // Active high
        3,              // Output
        0,              // External trigger
        0,              // Trigger select 0
        0,              // Trigger polarity (active high)
        1,              // Enable on timer N trigger high
        0,              // Disable on timer N trigger high
        0,              // Reset on timer N trigger high
        0,              // Decrement on FlexIO clock
        1,              // Output logic one when enabled and not reset
        0,              // Start bit 0
        1               // Stop bit 1
    );
    
    // Verify timer control register
    uint32_t expectedTimctl = (0 << 24) |    // TRGSEL
                             (0 << 23)  |      // TRGPOL
                             (0 << 22)  |      // TRGSRC
                             (3 << 16)  |      // PINCONFIG
                             (0 << 8) |      // PINSEL
                             (0 << 7)  |      // PINPOL
                             1;               // TIMOD
    TEST_ASSERT_EQUAL_HEX32(expectedTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex]);
    
    // Verify timer configuration register
    uint32_t expectedTimcfg = (1 << 24)  |      // TIMOUT
                             (0 << 20) |      // TIMDEC
                             (0 << 16) |      // TIMRST
                             (0 << 12) |      // TIMDIS
                             (1 << 8) |    // TIMENA
                             (1 << 4) |             // TIMSTOP
                             0;     // TIMSTRT
    TEST_ASSERT_EQUAL_HEX32(expectedTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex]);
    
    // Clean up
    flexIO1->releaseTimer(timerIndex);
}

// Test invalid timer index
void test_timer_config_invalid_index(void) {
    uint8_t timerIndex = FlexIOHandler::CNT_TIMERS; // Invalid index
    
    // Save current register values
    uint32_t originalTimctl = flexIO1->getFlexIO()->TIMCTL[0];
    uint32_t originalTimcfg = flexIO1->getFlexIO()->TIMCFG[0];
    
    // Try to configure with invalid index
    flexIO1->configureTimer(
        timerIndex, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1
    );
    
    // Verify registers haven't changed
    TEST_ASSERT_EQUAL_HEX32(originalTimctl, flexIO1->getFlexIO()->TIMCTL[0]);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg, flexIO1->getFlexIO()->TIMCFG[0]);
}

// Test invalid mode
void test_timer_config_invalid_mode(void) {
    uint8_t timerIndex = 0;
    
    // Request the timer first
    TEST_ASSERT_EQUAL_HEX8(0, flexIO1->requestTimer(timerIndex));
    
    // Save current register values
    uint32_t originalTimctl = flexIO1->getFlexIO()->TIMCTL[timerIndex];
    uint32_t originalTimcfg = flexIO1->getFlexIO()->TIMCFG[timerIndex];
    
    // Try to configure with invalid mode
    flexIO1->configureTimer(
        timerIndex, 4, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1
    );
    
    // Verify registers haven't changed
    TEST_ASSERT_EQUAL_HEX32(originalTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex]);
    
    // Clean up
    flexIO1->releaseTimer(timerIndex);
}

// Test invalid trigger source
void test_timer_config_invalid_trigger_source(void) {
    uint8_t timerIndex = 0;
    
    // Request the timer first
    TEST_ASSERT_EQUAL_HEX8(0, flexIO1->requestTimer(timerIndex));
    
    // Save current register values
    uint32_t originalTimctl = flexIO1->getFlexIO()->TIMCTL[timerIndex];
    uint32_t originalTimcfg = flexIO1->getFlexIO()->TIMCFG[timerIndex];
    
    // Try to configure with invalid trigger source
    flexIO1->configureTimer(
        timerIndex, 1, 0, 0, 3, 2, 0, 0, 1, 0, 0, 0, 1, 0, 1
    );
    
    // Verify registers haven't changed
    TEST_ASSERT_EQUAL_HEX32(originalTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex]);
    
    // Clean up
    flexIO1->releaseTimer(timerIndex);
}

// Test invalid trigger select
void test_timer_config_invalid_trigger_select(void) {
    uint8_t timerIndex = 0;
    
    // Request the timer first
    TEST_ASSERT_EQUAL_HEX8(0, flexIO1->requestTimer(timerIndex));
    
    // Save current register values
    uint32_t originalTimctl = flexIO1->getFlexIO()->TIMCTL[timerIndex];
    uint32_t originalTimcfg = flexIO1->getFlexIO()->TIMCFG[timerIndex];
    
    // Try to configure with invalid trigger select
    flexIO1->configureTimer(
        timerIndex, 1, 0, 0, 3, 0, 64, 0, 1, 0, 0, 0, 1, 0, 1
    );
    
    // Verify registers haven't changed
    TEST_ASSERT_EQUAL_HEX32(originalTimctl, flexIO1->getFlexIO()->TIMCTL[timerIndex]);
    TEST_ASSERT_EQUAL_HEX32(originalTimcfg, flexIO1->getFlexIO()->TIMCFG[timerIndex]);
    
    // Clean up
    flexIO1->releaseTimer(timerIndex);
}
