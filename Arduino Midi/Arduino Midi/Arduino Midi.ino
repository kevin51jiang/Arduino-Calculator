/*
 Name:		Arduino_Midi.ino
 Created:	5/16/2019 1:00:41 🍺
 Author:	Kevin J
*/

// Regsiters

#include <splash.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <Key.h>
#include <Adafruit_GFX.h>
 
#define OLED_Address 0x3C
Adafruit_SSD1306 oled(1);
 


byte reg1 = B00000000,
	reg2 = B00000000;

// Interface with Display

// Interface with keypad
char checkKeypad() {

}
// Interface with adder

//convert binary to integer

int counter = 0;
void setup() {
	pinMode(2, INPUT_PULLUP);
	oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
}

// the loop function runs over and over again until power down or reset
void loop() {
	//char instruction = checkKeypad();
	//if(instruction < )
	oled.clearDisplay();
	oled.setTextColor(WHITE);
	oled.setCursor(0, 0);
	oled.println(counter);
	oled.display();

	if (digitalRead(2) == LOW) {
		counter++;
	}

	delay(1);//delay
}
