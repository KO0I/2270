#include <Arduino.h>

void setup();
void loop();
#line 1 "src/sketch.ino"
/******************************************************************
*  ECEN2830 motor speed control example (left wheel only)
******************************************************************/

// define pins
const int pinCW_Right = 2;   // 
const int pinCC_Right = 4;   // 
const int pinON = 6;         // connect pin 6 to ON/OFF switch, active HIGH
const int pinCW_Left = 7;    // connect pin 7 to clock-wise PMOS gate
const int pinCC_Left = 8;    // connect pin 8 to counter-clock-wise PMOS gate
const int pinSpeed = 9; // connect pin 9 to speed reference
int i = 254;

// setup pins and initial values
void setup() {
  pinMode(pinON,INPUT);
  pinMode(pinCW_Right,OUTPUT);
  pinMode(pinCC_Right,OUTPUT);
  pinMode(pinCW_Left,OUTPUT);
  pinMode(pinCC_Left,OUTPUT);
  pinMode(pinSpeed,OUTPUT);
  pinMode(13,OUTPUT);             // on-board LED
  digitalWrite(pinCW_Left,LOW);   // stop clockwise
  digitalWrite(pinCC_Left,LOW);   // stop counter-clockwise
  analogWrite(pinSpeed,i); // set speed reference, duty-cycle =
}

void loop() {
    digitalWrite(13,LOW);                     // turn LED off
    do {} while (digitalRead(pinON) == LOW);  // wait for ON switch
    /*
  //  first rotation 
    digitalWrite(13,HIGH);                    // turn LED on
    delay(500);                              // wait 1 second
    digitalWrite(pinCC_Left,HIGH);            // go clockwise
    digitalWrite(pinCC_Right,HIGH);           // go clockwise
    delay(800);               
    digitalWrite(pinCC_Left,LOW);             // stop
    digitalWrite(pinCC_Right,LOW);            // stop
    delay(100);
    digitalWrite(pinCW_Left,HIGH);            // go clockwise
    digitalWrite(pinCW_Right,HIGH);           // go clockwise
    delay(800);               
    digitalWrite(pinCW_Left,LOW);             // stop
    digitalWrite(pinCW_Right,LOW);            // stop
    analogWrite(pinSpeed,i=i-10);
    if(i<=195) i=254;
    */
    
    
    //  Speed Test
    digitalWrite(13,HIGH);                    //  turn LED on
    digitalWrite(pinCC_Left,HIGH);            // go clockwise
    digitalWrite(pinCC_Right,HIGH);           // go clockwise
    while(i>=106){
      analogWrite(pinSpeed, i);
      delay(45);
      i--;
    }
      while(i<=230){
        analogWrite(pinSpeed, i);
        delay(5);
        i++;
      }
   // digitalWrite(PinCW_Left,LOW);
   // digitalWrite(PinCW_Right,LOW);
    }
