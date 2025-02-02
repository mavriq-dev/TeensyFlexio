#include <FlexIO_t4.h>
#include <TeensyFlexSerial.h>
#include <MIDI.h>

// Demonstrate using FlexSerial for MIDI OUT, inspired by:
// https://forum.pjrc.com/index.php?threads/75818/

TeensyFlexSerial myport(32, -1, 2); // MIDI OUT on pin 32
MIDI_CREATE_INSTANCE(TeensyFlexSerial, myport, mymidi);

void setup() {
  myport.setClock(9795918);
  mymidi.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  mymidi.sendControlChange(9, 65, 2);
  delay(1000);
}