#include <Arduino.h>
#include "TeensyFlexSerial.h"

TeensyFlexSerial* flexIO;
elapsedMillis output_timer;
const int baud = 115200;


void setup() {
    while (!Serial && millis() < 4000);
    Serial.begin(baud);
    Serial.println("FlexSerial Loopback Example");
    Serial1.begin(baud);
    flexIO = new TeensyFlexSerial(5, 6, 1, -1, -1, 2);
    flexIO->begin();
    output_timer = 0;
}

void loop() {
    int ch;
    // periodically transmit a message on FlexSerial
    if (output_timer >= 1000) {
        output_timer = 0;
        digitalToggle(13);
        // flexeIO->writeString("abcdefghijklmnopqrstuvwxyz\n");
        flexIO->println("abcdefghijklmnopqrstuvwxyz");
    }

    // when FlexSerial receives the echoed message, send it to USB Serial
    if (flexIO->available()) {
        ch = flexIO->read();
        Serial.write(ch);
    }
}


