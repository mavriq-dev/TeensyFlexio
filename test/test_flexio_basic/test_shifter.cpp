#include <Arduino.h>
#include <unity.h>
#include "TeensyFlexIO.h"
#include "run_tests.h"

void test_request_default_shifter(void) {
    // Create new FlexIO instance before each test
    // TeensyFlexIO* flexIO1 = new TeensyFlexIO(TeensyFlexIO::FLEXIO1);
    
    // Should get first available shifter
    int8_t shifter = flexIO1->requestShifter();
    TEST_ASSERT_GREATER_OR_EQUAL_INT8(0, shifter);
    
    // Clean up
    flexIO1->releaseShifter(shifter);

}

void test_request_specific_shifter(void) {
    // Create new FlexIO instance before each test
    // TeensyFlexIO* flexIO1 = new TeensyFlexIO(TeensyFlexIO::FLEXIO1);
    
    // Request shifter 0 specifically
    int8_t shifter = flexIO1->requestShifter(0);
    TEST_ASSERT_EQUAL_INT8(0, shifter);
    
    // Clean up
    flexIO1->releaseShifter(shifter);

}

void test_request_unavailable_shifter(void) {
    // Create new FlexIO instance before each test
    // TeensyFlexIO* flexIO1 = new TeensyFlexIO(TeensyFlexIO::FLEXIO1);
    
    // First claim shifter 0
    int8_t first = flexIO1->requestShifter(0);
    TEST_ASSERT_EQUAL_INT8(0, first);
    
    // Try to claim it again
    int8_t second = flexIO1->requestShifter(0);
    TEST_ASSERT_EQUAL_INT8(-1, second);
    
    // Clean up
    flexIO1->releaseShifter(first);
 ;
}

void test_release_and_reclaim_shifter(void) {
    // Create new FlexIO instance before each test
    // TeensyFlexIO* flexIO1 = new TeensyFlexIO(TeensyFlexIO::FLEXIO1);
    
    // Claim a specific shifter
    int8_t shifter = flexIO1->requestShifter(0);
    TEST_ASSERT_EQUAL_INT8(0, shifter);
    
    // Release it
    flexIO1->releaseShifter(shifter);
    
    // Should be able to claim it again
    shifter = flexIO1->requestShifter(0);
    TEST_ASSERT_EQUAL_INT8(0, shifter);
    
    // Clean up
    flexIO1->releaseShifter(shifter);

}

void test_multiple_shifter_requests(void) {
    // Create new FlexIO instance before each test
    // TeensyFlexIO* flexIO1 = new TeensyFlexIO(TeensyFlexIO::FLEXIO1);
    
    // Should be able to get multiple different shifters
    int8_t shifter1 = flexIO1->requestShifter();
    TEST_ASSERT_GREATER_OR_EQUAL_INT8(0, shifter1);
    
    int8_t shifter2 = flexIO1->requestShifter();
    TEST_ASSERT_GREATER_OR_EQUAL_INT8(0, shifter2);
    TEST_ASSERT_NOT_EQUAL_INT8(shifter1, shifter2);
    
    // Clean up
    flexIO1->releaseShifter(shifter1);
    flexIO1->releaseShifter(shifter2);

}
