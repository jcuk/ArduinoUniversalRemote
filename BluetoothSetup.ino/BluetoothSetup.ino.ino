
//HM-10 bluetooth setup.
// Pin connections from BT moule to Arduino:
// VCC --> 5v
// GND --> 0v
// Rx --> Pin 11
// Tx --> Pin 10

//Commands can be issued through the serial monitor
// Serial monitor settings are 9600Baud, no line endings

// Simple commands:
// AT  should return ATOK
// AT+NAME? returns the modules current name
// AT+NAME[name] sets the module name to [name]

#include <SoftwareSerial.h>
#define rxPin 10
#define txPin 11
SoftwareSerial mySerial(rxPin, txPin); // RX / TX of the arduino
char myChar ;
void setup() {
  Serial.begin(9600);   
  Serial.println("AT");
  mySerial.begin(9600);
  mySerial.println("AT");
}
void loop() {
  while (mySerial.available()) {
    myChar = mySerial.read();
    Serial.print(myChar);
  }
 while (Serial.available()) {
    myChar = Serial.read();
    Serial.print(myChar); //echo
    mySerial.print(myChar);
  }
}
