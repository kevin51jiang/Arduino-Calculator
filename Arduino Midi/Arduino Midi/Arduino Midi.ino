/*
 Name:		Arduino_Midi.ino
 Created:	5/16/2019 1:00:41 🍺
 Author:	Kevin J
*/


#include <splash.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>
#include <Key.h>
#include <Adafruit_GFX.h>

// I2C address of the OLED LCD
#define OLED_Address 0x3C

// Full Adder Pin Configs
#define PIN_A 2
#define PIN_B 3
#define CARRY_IN 4
#define CARRY_OUT  A6
#define SUM A7

// Time delay between each calculation in ms
#define CALC_DELAY 500

// greater than ~1 volt
#define ANALOG_READ_THRESH 200
const byte ROWS = 4;
const byte COLS = 4;

// declare key values
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'+', '0', '=', 'D'}
};

// Pin Declarations for Keypad
byte rowPins[ROWS] = { 12, 11, 10, 9 };
byte colPins[COLS] = { 8, 7, 6, 5 };

//instantiate keypad object
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


/*
 Phases:
 0: choose the first number
 1: choose the second number
 2: add up the numbers
 3: display result
 */
int phase = 0;

//variable for result
long result = 0;

//variables for the 2 operands
String num1 = "?", num2 = "?";


// template for the reimplementation of the Pair class from normal C++
// https://github.com/zacsketches/Arduino_pair
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

	// wait for even longer
	delay(CALC_DELAY / 2);

	//cleanup
	digitalWrite(PIN_A, 0);
	digitalWrite(PIN_B, 0);
	digitalWrite(CARRY_IN, 0);


	return results;
}

// adds 2 integers
long add(long num1, long num2) {
	//by default, everything is 0
	bool carry = 0;
	long results = 0;

	// run it through the adder a max of 32 times (maximum size of an integer)
	for (int i = 0; i < 32; i++) {
		// use the add_bits function to add each bit read from the operands
		Pair<bool, bool> temp_pair = add_bits(bitRead(num1, i), bitRead(num2, i), carry);

		// if the sum is 1, then set the result's bit at index i to true
		if (temp_pair.first() == 1) {
			bitSet(results, i);
		}

		// set the carry boolean to the carry parameter that's returned from adding
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

		Serial.print(results);
		
		Serial.println(' ');
	}
	//END DEBUG

	//make it impossible to overflow
	if (carry == 1) {
		bitSet(results, 32);
	}
	Serial.print("TOTAL: ");
	Serial.println(results);
	return results;
}


//convert binary to integer
void setup() {
	//for serial communication
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
	case 0: // for 1st number
		//get info and print out current status
		oled.setCursor(0, 0);
		oled.println(num1 + " + " + num2);
		oled.setCursor(0, 20);
		command = keypad.getKey(); // get whatever key is pressed (if applicable)
		oled.println(command);
		oled.display();

		//allow user to see whatever was entered
		delay(100);

		//if there was a key pressed
		if (command > 0) {

			if (command >= '0' && command <= '9') { // Entered a number
				if (num1.equals("?")) { //if results is currently "?", replace it with whatever number is entered
					num1 = String(command);
				}
				else { // or else concatenate it to the end
					num1 += command;
				}
			}
			else if (command == '+') { //presses the + button
				phase++;
				if (num1.equals("?")) { //if no number has been entered, set it to the default value of 0
					num1 = "0";
				}
			}
		}
		break;

	case 1: // for 2nd operand
		//get info and print out current status
		oled.setCursor(0, 0);
		oled.println(num1 + " + " + num2);
		oled.setCursor(0, 20);
		command = keypad.getKey();// get whatever key is pressed (if applicable)
		oled.println(command);
		oled.display();

		//allow user to see whatever was entered
		delay(100);

		//if there was a key pressed
		if (command > 0) {

			if (command >= '0' && command <= '9') { //user enteres a number 

				//replaces the "?" with entry, or concatenates it
				if (num2.equals("?")) {
					num2 = String(command);
				} else {
					num2 += command;
				}
			}
			else if (command == '=') { //presses the = button
				if (num2.equals("?")) { //if no number has been entered, set it to the default value of 0
					num2 = "0";
				}

				phase++;
			}
		}
		break;

	case 2: //add the actual numbers
		oled.setCursor(0, 0);
		oled.println(num1 + " + " + num2 + " = ?");
		oled.setCursor(0, 20);
		oled.println("Adding...");
		oled.display(); //display message to show that it is in the process of adding

		//convert operands to longs (toInt returns a long) and add them using the full adder
		result = add(num1.toInt(), num2.toInt()); 
		phase++; // continue to next stage
		break;
	case 3:
		oled.setCursor(0, 0);
		oled.println(num1 + " + " + num2 + " = " + result);
		oled.display();
		delay(3000); //display all the operands and the sum for 3 seconds

		//reset everything to default state so that it can be used again
		phase = 0;
		num1 = "?";
		num2 = "?";
		result = 0;
		break;
	}
}
