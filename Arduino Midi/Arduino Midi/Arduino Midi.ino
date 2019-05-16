/*
 Name:		Arduino_Midi.ino
 Created:	5/16/2019 1:00:41 🍺
 Author:	Kevin J
*/

// the setup function runs once when you press reset or power the board
#include <MIDI.h>


MIDI_CREATE_CUSTOM_INSTANCE();

void setup() {
	MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages

}

// the loop function runs over and over again until power down or reset
void loop() {
	// Send note 42 with velocity 127 on channel 1
	MIDI.sendNoteOn(42, 127, 1);
	delay(1000);
	MIDI.read();
}
