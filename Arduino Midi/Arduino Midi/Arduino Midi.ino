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
#define PIN_A 2
#define PIN_B 3
#define CARRY_IN 4
#define SUM A6
#define CARRY_OUT  A7

#define CALC_DELAY 1500

// greater than ~1 volt
#define ANALOG_READ_THRESH 200
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'+', '0', '=', 'D'}
};

// Pin Declarations for Keypad
byte rowPins[ROWS] = { 12, 11, 10, 9 };
byte colPins[COLS] = { 8, 7, 6, 5 };

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


/*
 Phases:
 0: choose the first number
 1: choose the second number
 2: add up the numbers 
 */
int phase = 0; 
int result = 0;

String num1 = "?", num2 = "?";

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

//init LCD display
Adafruit_SSD1306 oled(1);
 

// Interface with adder
// Returns Pair<Sum, Carry>
Pair<bool, bool> add_bits(bool a, bool b, bool cin) {

	//output to adder
	digitalWrite(PIN_A, a);
	digitalWrite(PIN_B, b);
	digitalWrite(CARRY_IN, cin);

	//wait so that transistors have time to process
	delay(CALC_DELAY);

	//read in values and assign to pair
	Pair<bool, bool> results((analogRead(SUM) > ANALOG_READ_THRESH) ? true : false, 
								(analogRead(CARRY_OUT) > ANALOG_READ_THRESH) ? true : false);

	//Pair<bool, bool> results(digitalRead(SUM), digitalRead(CARRY_OUT));

	delay(CALC_DELAY / 2);

	//cleanup
	digitalWrite(PIN_A, 0);
	digitalWrite(PIN_B, 0);
	digitalWrite(CARRY_IN, 0);


	return results;
}


int add(int num1, int num2) {
	bool carry = 0;
	unsigned int result = 0;

	// run it through the adder a max of 32 times (maximum size of an integer)
	for (int i = 0; i < 32; i++) {
		Pair<bool, bool> temp_pair = add_bits(bitRead(num1, i), bitRead(num2, i), carry);
		if (temp_pair.first() == 1) {
			bitSet(result, i);
		}
		carry = temp_pair.second();


		/*
		DEBUG
		*/

		Serial.print(i);
		Serial.print(" / ");

		Serial.print(bitRead(num1, i));
		Serial.print(", ");
		Serial.print(bitRead(num2, i));
		Serial.print(", ");
		Serial.print(carry);

		Serial.print(" / ");

		Serial.print(temp_pair.first());
		Serial.print(", ");
		Serial.print(temp_pair.second());
		
		Serial.print(" / ");

		Serial.print(result);
		
		Serial.println(' ');
	}

	//make it impossible to overflow
	if (carry == 1) {
		bitSet(result, 32);
	}

	return result;
}


//convert binary to integer
void setup() {
	Serial.begin(9600);

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
	char command;
	oled.clearDisplay();
	oled.setTextColor(WHITE);


	switch (phase) {
		case 0:
			oled.setCursor(0, 0);
			oled.println(num1 + " + " + num2);
			oled.setCursor(0, 20);
			command = keypad.getKey();
			oled.println(command);
			//oled.println("Enter the first number: " + num1);
			oled.display();
			
			//Serial.println(command);

			delay(100);

			//if there was a key pressed
			if (command > 0) {
				// Enters a keypad button
				if (command >= '0' && command <= '9') {
					if (num1.equals("?")) {
						num1 = String(command);
					} else {
						num1 += command;
					}
				}
				else if (command == '+') { //presses the + button
					phase++;
					if (num1.equals("?")) {
						num1 = "0";
					}
				}
			}
			break;
		case 1:
			oled.setCursor(0, 0);
			oled.println(num1 + " + " + num2);
			oled.setCursor(0, 20);
			//oled.println("Enter the first number: " + num1);
			command = keypad.getKey();
			oled.println(command);
			oled.display();
			
			//delay
			delay(100);
			
			//if there was a key pressed
			if (command > 0) {
				// Enters a keypad button
				if (command >= '0' && command <= '9') {
					if (num2.equals("?")) {
						num2 = String(command);
					}
					else {
						num2 += command;
					}
				}
				else if (command == '+') { //presses the + button
					phase++;
					if (num2.equals("?")) {
						num2 = "0";
					}
				}
			}
			break;

		case 2: //add
			oled.setCursor(0, 0);
			oled.println(num1 + " + " + num2 + " = ?");
			oled.setCursor(0, 20);
			oled.println("Adding...");
			oled.display();
			result = add(num1.toInt(), num2.toInt());
			phase++;
			break;
		case 3:
			oled.setCursor(0, 0);
			oled.println(num1 + " + " + num2 + " = " + result);
			oled.display();
			delay(2000);

			//reset
			phase = 0;
			num1 = "?";
			num2 = "?";
			result = 0;
			break;
	}
	delay(2);
}
