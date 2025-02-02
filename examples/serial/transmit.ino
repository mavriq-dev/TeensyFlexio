#include <FlexSerial.h>
#include "TeensyFlexSerial.h"

#define TX_PIN 5
#define TX_FLEX_IO_INDEX 1
#define TX_FLEX_IO_TIMER -1
#define TX_FLEX_IO_SHIFTER -1

TeensyFlexSerial* flexSerial = new TeensyFlexSerial(TX_PIN, -1, TX_FLEX_IO_INDEX, TX_FLEX_IO_SHIFTER, TX_FLEX_IO_TIMER);
int count_bytes_still_expected = 0;

void serialEvent1() {
  int ch;
  while ((ch = Serial1.read()) != -1) {
    if (count_bytes_still_expected > -10) {
      Serial.write(ch);
      count_bytes_still_expected--;
    } else if (count_bytes_still_expected == -10) {
      Serial.println("*** Error receiving too many bytes ***");
      count_bytes_still_expected--;
    }
  }
}

void setup() {
  // pinMode(13, OUTPUT);
  while (!Serial && millis() < 4000);
  Serial.begin(115200);
  Serial1.begin(115200);  // lets start up Serial1, to see if we can receive anything from our FlexSerial
  delay(500);
  flexSerial->begin(115200);
  

  Serial.printf("Connect jumper from pin %d to 0)\n", TX_PIN);
  Serial.println("End Setup");
  count_bytes_still_expected = 32; // close enough...
  flexSerial->println("Enter something to have it echo");
}

void loop() {

}
