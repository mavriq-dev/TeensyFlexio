#include "run_tests.h"

TeensyFlexIO* flexIO1 = nullptr;
TeensyFlexIO* flexIO2 = nullptr;
TeensyFlexIO* flexIO3 = nullptr;

void setUp(void) {
    flexIO1 = new TeensyFlexIO();
    flexIO1->begin(TeensyFlexIO::FLEXIO1);
    flexIO2 = new TeensyFlexIO( );
    flexIO2->begin(TeensyFlexIO::FLEXIO2);
    flexIO3 = new TeensyFlexIO();
    flexIO3->begin(TeensyFlexIO::FLEXIO3);
}

void tearDown(void) {
    if (flexIO1) {
        delete flexIO1;
        flexIO1 = nullptr;
    }
}

void run_shifter_tests(void) {
    RUN_TEST(test_request_default_shifter);
    RUN_TEST(test_request_specific_shifter);
    RUN_TEST(test_request_unavailable_shifter);
    RUN_TEST(test_release_and_reclaim_shifter);
    RUN_TEST(test_multiple_shifter_requests);
}

void run_shifter_config_tests(void) {
    RUN_TEST(test_basic_shifter_config);
    RUN_TEST(test_spi_mode_shifter_config);
    RUN_TEST(test_uart_mode_shifter_config);
    RUN_TEST(test_shifter_config_max_values);
    RUN_TEST(test_shifter_config_field_boundaries);
    RUN_TEST(test_shifter_config_struct_max_values);
    RUN_TEST(test_read_shifter_config);
}

void run_timer_tests(void) {
    RUN_TEST(test_request_default_timer);
    RUN_TEST(test_request_specific_timer);
    RUN_TEST(test_request_unavailable_timer);
    RUN_TEST(test_release_and_reclaim_timer);
    RUN_TEST(test_multiple_timer_requests);
}

void run_timer_config_tests(void) {
    RUN_TEST(test_basic_timer_config);
    RUN_TEST(test_spi_mode_timer_config);
    RUN_TEST(test_uart_mode_timer_config);
    RUN_TEST(test_timer_config_max_values);
    RUN_TEST(test_timer_config_field_boundaries);
    RUN_TEST(test_timer_config_valid);
    RUN_TEST(test_timer_config_invalid_index);
    RUN_TEST(test_timer_config_invalid_mode);
    RUN_TEST(test_timer_config_invalid_trigger_source);
    RUN_TEST(test_timer_config_invalid_trigger_select);
    RUN_TEST(test_timer_config_struct_max_values);
    RUN_TEST(test_read_timer_config);
}

void run_pin_config_tests(void) {
    RUN_TEST(test_flexio1_valid_pins);
    RUN_TEST(test_flexio2_valid_pins);
    RUN_TEST(test_flexio3_valid_pins);
    RUN_TEST(test_invalid_pin_numbers);
}

void run_timer_enable_tests(void) {
    RUN_TEST(test_timer_basic_enable_disable);
    RUN_TEST(test_timer_clock_settings);
}

void setup() {
    delay(2000);  // Allow board to settle
    UNITY_BEGIN();
    
    // Run all test groups
    run_shifter_tests();
    run_shifter_config_tests();
    run_timer_tests();
    run_timer_config_tests();
    run_pin_config_tests();
    run_timer_enable_tests();
    
    UNITY_END();
}

void loop() {
    // Empty loop
}
