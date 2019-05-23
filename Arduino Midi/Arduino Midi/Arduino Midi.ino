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

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'+', '0', '=', 'D'}
};

//TO CHANGE
byte rowPins[ROWS] = { 9, 8, 7, 6 };
byte colPins[COLS] = { 5, 4, 3, 2 };

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


/*
 Phases:
 0: choose the first number
 1: choose the second number
 2: add up the numbers 
 */
int phase = 0; 

String num1 = "", num2 = "";

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
	oled.clearDisplay();
	oled.setTextColor(WHITE);
	switch (phase) {
		case 0:
			oled.setCursor(0, 0);
			oled.println(num1 + " + " + num2);
			oled.setCursor(0, 20);
			oled.println("Enter the first number: " + num1);
			oled.display();
			char command = keypad.getKey();
			
			//if there was a key pressed
			if (command > 0) {
				// Enters a keypad button
				if (command >= '0' && command <= '9') {
					num1 += command;
				}
				else if (command == '+') { //presses the + button
					phase++;
				}
			}
			break;
		case 1:
			oled.setCursor(0, 0);
			oled.println(num1 + " + " + num2);
			oled.setCursor(0, 20);
			oled.println("Enter the first number: " + num1);
			oled.display();
			char command = keypad.getKey();

			//if there was a key pressed
			if (command > 0) {
				// Enters a keypad button
				if (command >= '0' && command <= '9') {
					num2 += command;
				}
				else if (command == '+') { //presses the + button
					phase++;
				}
			}
			break;

		case 2: //add
			oled.setCursor(0, 0);
			oled.println(num1 + " + " + num2 + " = ?");
			oled.setCursor(0, 20);
			oled.println("Adding...");
			oled.display();
			add(num1.toInt(), num2.toInt());
			break;
		case 3:
			oled.setCursor(0, 0);
			
	}
	delay(2);
}
