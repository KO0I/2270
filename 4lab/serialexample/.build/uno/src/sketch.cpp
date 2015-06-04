#include <Arduino.h>

void setup();
void loop();
#line 1 "src/sketch.ino"
/*
ECEN2830 example of displaying a value using Serial Monitor 
Open the Serial Monitor window: Tools > Serial Monitor
*/

// define pins
const int pinON = 6;         // connect pin 6 to ON/OFF switch via 1k resistor, active HIGH

void setup() {
  Serial.begin(9600);       // start serial communication via USB at 9600 bits per second (baud)
  pinMode(pinON, INPUT);    // set on/off switch pin as input
}

void loop() {
  int onoff_switch = digitalRead(pinON);   // read switch on/off state
  Serial.println(onoff_switch);            // send value as ASCII-encoded decimal 
  delay(500);                              // wait 0.5 seconds
}



