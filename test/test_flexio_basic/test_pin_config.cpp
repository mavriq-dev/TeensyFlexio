#include "run_tests.h"
#include "TeensyFlexIO.h"
#include <unity.h>

// Test valid pins for FlexIO1
void test_flexio1_valid_pins() {
    // TeensyFlexIO flexio(TeensyFlexIO::FLEXIO1);
    
    // FlexIO1 valid pins: 2,3,4,5,33
    TEST_ASSERT_TRUE(flexIO1->setPinFlexioMode(2));
    TEST_ASSERT_TRUE(flexIO1->setPinFlexioMode(3));
    TEST_ASSERT_TRUE(flexIO1->setPinFlexioMode(4));
    TEST_ASSERT_TRUE(flexIO1->setPinFlexioMode(5));
    TEST_ASSERT_TRUE(flexIO1->setPinFlexioMode(33));
    
    // Test some invalid pins
    TEST_ASSERT_FALSE(flexIO1->setPinFlexioMode(6));  // FlexIO2 pin
    TEST_ASSERT_FALSE(flexIO1->setPinFlexioMode(14)); // FlexIO3 pin
    TEST_ASSERT_FALSE(flexIO1->setPinFlexioMode(0));  // Not a FlexIO pin
}

// Test valid pins for FlexIO2
void test_flexio2_valid_pins() {
    // TeensyFlexIO flexio(TeensyFlexIO::FLEXIO2);
    
    // FlexIO2 valid pins: 6,7,8,9,10,11,12,13,32
    TEST_ASSERT_TRUE(flexIO2->setPinFlexioMode(6));
    TEST_ASSERT_TRUE(flexIO2->setPinFlexioMode(7));
    TEST_ASSERT_TRUE(flexIO2->setPinFlexioMode(8));
    TEST_ASSERT_TRUE(flexIO2->setPinFlexioMode(9));
    TEST_ASSERT_TRUE(flexIO2->setPinFlexioMode(10));
    TEST_ASSERT_TRUE(flexIO2->setPinFlexioMode(11));
    TEST_ASSERT_TRUE(flexIO2->setPinFlexioMode(12));
    TEST_ASSERT_TRUE(flexIO2->setPinFlexioMode(13));
    TEST_ASSERT_TRUE(flexIO2->setPinFlexioMode(32));
    
    // Test some invalid pins
    TEST_ASSERT_FALSE(flexIO2->setPinFlexioMode(2));  // FlexIO1 pin
    TEST_ASSERT_FALSE(flexIO2->setPinFlexioMode(14)); // FlexIO3 pin
    TEST_ASSERT_FALSE(flexIO2->setPinFlexioMode(1));  // Not a FlexIO pin
}

// Test valid pins for FlexIO3
void test_flexio3_valid_pins() {
    // TeensyFlexIO flexio(TeensyFlexIO::FLEXIO3);
    
    // FlexIO3 valid pins: 7,8,14,15,16,17,18,19,20,21,22,23,26,27
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(7));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(8));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(14));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(15));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(16));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(17));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(18));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(19));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(20));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(21));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(22));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(23));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(26));
    TEST_ASSERT_TRUE(flexIO3->setPinFlexioMode(27));
    
    // Test some invalid pins
    TEST_ASSERT_FALSE(flexIO3->setPinFlexioMode(2));  // FlexIO1 pin
    TEST_ASSERT_FALSE(flexIO3->setPinFlexioMode(6));  // FlexIO2 pin
    TEST_ASSERT_FALSE(flexIO3->setPinFlexioMode(1));  // Not a FlexIO pin
}

// Test invalid pin numbers
void test_invalid_pin_numbers() {
    // TeensyFlexIO flexio(TeensyFlexIO::FLEXIO1);
    
    // Test boundary conditions and invalid pins
    TEST_ASSERT_FALSE(flexIO1->setPinFlexioMode(0xFF));  // Invalid pin
    TEST_ASSERT_FALSE(flexIO1->setPinFlexioMode(100));   // Out of range
    TEST_ASSERT_FALSE(flexIO1->setPinFlexioMode(-1));    // Negative pin number
}
