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

// Full Adder
#define PIN_A 5
#define PIN_B 6
#define CARRY_IN 7
#define SUM 8
#define CARRY_OUT  9

#define CALC_DELAY 50


template<class M, class N>
struct Pair {
	M val_1;
	N val_2;

	Pair(M v1, N v2)
		:val_1(v1), val_2(v2) {}

	//use default values for copy construction and assignment

	M first() { return val_1; }
	const M first() const { return val_1; }

	N second() { return val_2; }
	const N second() const { return val_2; }
};


Adafruit_SSD1306 oled(1);
 


byte reg1 = B00000000,
	reg2 = B00000000;

// Interface with Display

// Interface with keypad
char checkKeypad() {

}


int add(unsigned short num1, unsigned short num2) {
	//convert to twos complement
	bool carry = 0;
	unsigned short result = 0;
	for (int i = 0; i < 32; i++) {
		Pair<bool, bool> temp_pair = add_bits(bitRead(num1, i), bitRead(num2, i), carry);
		if (temp_pair.first() == 1) {
			bitSet(result, i);
		}
		carry = temp_pair.second();
	}
	//make it impossible to overflow
	if (carry == 1) {
		bitSet(result, 32);
	}

	return result;
}


// Interface with adder
// Returns Pair<Sum, Carry>
Pair<bool, bool> add_bits(bool a, bool b, bool cin) {

	//output to adder
	digitalWrite(PIN_A, a);
	digitalWrite(PIN_B, b);
	digitalWrite(CARRY_IN, cin);

	//wait so that transistors have time to process
	delay(CALC_DELAY);
	
	Pair<bool, bool> results(digitalRead(SUM), digitalRead(CARRY_OUT));

	delay(CALC_DELAY / 2);

	//cleanup
	digitalWrite(PIN_A, 0);
	digitalWrite(PIN_B, 0);
	digitalWrite(CARRY_IN, 0);


	return results;
}


//convert binary to integer

int counter = 0;
void setup() {
	//button
	pinMode(2, INPUT_PULLUP);

	//adder input
	pinMode(PIN_A, OUTPUT);
	pinMode(PIN_B, OUTPUT);
	pinMode(CARRY_IN, OUTPUT);

	//adder output
	pinMode(SUM, INPUT);
	pinMode(CARRY_OUT, INPUT);

	//oled
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
