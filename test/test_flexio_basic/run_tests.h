#ifndef RUN_TESTS_H
#define RUN_TESTS_H

#include <Arduino.h>
#include <unity.h>
#include "TeensyFlexIO.h"

// Global FlexIO instance used by all tests
extern TeensyFlexIO* flexIO1;
extern TeensyFlexIO* flexIO2;
extern TeensyFlexIO* flexIO3;

// Shifter request/release tests
void test_request_default_shifter(void);
void test_request_specific_shifter(void);
void test_request_unavailable_shifter(void);
void test_release_and_reclaim_shifter(void);
void test_multiple_shifter_requests(void);

// Shifter configuration tests
void test_basic_shifter_config(void);
void test_spi_mode_shifter_config(void);
void test_uart_mode_shifter_config(void);
void test_shifter_config_max_values(void);
void test_shifter_config_field_boundaries(void);
void test_shifter_config_struct_max_values(void);
void test_read_shifter_config(void);

// Timer tests
void test_request_default_timer(void);
void test_request_specific_timer(void);
void test_request_unavailable_timer(void);
void test_release_and_reclaim_timer(void);
void test_multiple_timer_requests(void);

// Timer configuration tests
void test_basic_timer_config(void);
void test_spi_mode_timer_config(void);
void test_uart_mode_timer_config(void);
void test_timer_config_max_values(void);
void test_timer_config_field_boundaries(void);
void test_timer_config_valid(void);
void test_timer_config_invalid_index(void);
void test_timer_config_invalid_mode(void);
void test_timer_config_invalid_trigger_source(void);
void test_timer_config_invalid_trigger_select(void);
void test_timer_config_struct_max_values(void);
void test_read_timer_config(void);

// Pin configuration tests
void test_flexio1_valid_pins(void);
void test_flexio2_valid_pins(void);
void test_flexio3_valid_pins(void);
void test_invalid_pin_numbers(void);

// Timer enable/disable tests
void test_timer_basic_enable_disable(void);
void test_timer_clock_settings(void);

// Test group runners
void run_shifter_tests(void);
void run_shifter_config_tests(void);
void run_timer_tests(void);
void run_timer_config_tests(void);
void run_timer_enable_tests(void);

#endif // RUN_TESTS_H