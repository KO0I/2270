#include <Arduino.h>

void setup();
void count_Left();
void loop();
#line 1 "src/sketch.ino"
/*
ECEN2830 motor position control example (left wheel only)
*/

// define pins
const int pinON = 6;         // connect pin 6 to ON/OFF switch, active HIGH
const int pinCW_Left = 7;    // connect pin 7 to clock-wise PMOS gate
const int pinCC_Left = 8;    // connect pin 8 to counter-clock-wise PMOS gate
const int pinSpeed_Left = 9; // connect pin 9 to speed reference

// encoder counter variable
volatile int enc_count_Left = 0; // "volatile" means the variable is stored in RAM

// setup pins and initial values
void setup() {
  pinMode(pinON,INPUT);
  pinMode(pinCW_Left,OUTPUT);
  pinMode(pinCC_Left,OUTPUT);
  pinMode(pinSpeed_Left,OUTPUT);
  pinMode(13,OUTPUT);             // on-board LED
  digitalWrite(13,LOW);           // turn LED off
  digitalWrite(pinCW_Left,LOW);   // stop clockwise
  digitalWrite(pinCC_Left,LOW);   // stop counter-clockwise
  analogWrite(pinSpeed_Left,50);  // set speed reference, duty-cycle = 50/255
  /* 
    Connect left-wheel encoder output to pin 2 (external Interrupt 0) via a 1k resistor
    Rising edge of the encoder signal triggers an interrupt 
    count_Left is the interrupt service routine attached to Interrupt 0 (pin 2)
  */
  attachInterrupt(0, count_Left, RISING);
}

/*
  Interrupt 0 service routine
  Increment enc_count_Left on each rising edge of the 
  encoder signal connected to pin 2
*/ 
void count_Left(){
  enc_count_Left++;
}

void loop() {
  do {
    enc_count_Left = 0;                     // reset encoder counter to 0
  } while (digitalRead(pinON) == LOW);      // wait for ON switch
  digitalWrite(13,HIGH);                    // turn LED on
  digitalWrite(pinCW_Left,HIGH);            // go clockwise
  do {} while (enc_count_Left < 3*12*64);   // do nothing until wheel makes 3 rotations
  digitalWrite(pinCW_Left,LOW);             // stop
  digitalWrite(13,LOW);                     // turn LED off
  delay(1000);                              // wait 1 second
}
