#include <SoftwareSerial.h>

/*

This program only prints data received from the bluetooth module on the 
standard serial interface of the Arduino and sends data received from the 
standard serial interface of the Arduino to bluetooth module.

ATTENTION
The bluetooth module works with 3.3v signal level on the RX and TX pins.
Arduino Uno works with 5v signal level.
We will need a voltage divider on the RX pin to prevent the module 
from being damaged.

*/

// Defining digital pins 2(RX) and 3(TX) for serial communication with the bluetooth module
SoftwareSerial mySerial(2, 3); // RX, TX

char dev_name[] = "HC-06";
char dev_pass[] = "1234";

void setup() {
  // serial communication with the PC
  Serial.begin(9600);

  // serial communication with the bluetooth module
  mySerial.begin(38400);
  char AT_command[15];
  
  // Defining the bluetooth device name
  sprintf(AT_command,"AT+NAME%s",dev_name); 
  mySerial.print(AT_command);
  while(!mySerial.available());
  while(mySerial.available()) mySerial.read();

  // Defining the bluetooth device password for pairing
  // It has 4 digits
  sprintf(AT_command,"AT+PIN%s",dev_pass);
  mySerial.print(AT_command);
  while(!mySerial.available());
  while(mySerial.available()) mySerial.read();
}

void loop() {
  
  if (mySerial.available()) {
  	Serial.write(mySerial.read());
  }
  
  if (Serial.available()) {
  	mySerial.write(Serial.read());
  }

  delay(10);
  
}
