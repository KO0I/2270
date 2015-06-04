#include <Arduino.h>

void setup();
void stop();
void forward();
void left();
void right();
void loop();
#line 1 "src/sketch.ino"
/**************************************************
Counts the time that the comparitor is high
and then associates it with a function that turns
the wheels in some way.
**************************************************/
#define COMPARITOR 12
#define DEFAULT_SPEED 100
#define MAX_SPEED 230
// bounds for pulse length in units of us
#define DEBOUNCE  7000
#define BOUNDA    17000
#define BOUNDB    24900
#define BOUNDC    34000
//#define BOUNDD    36000
volatile unsigned char* portB = (volatile unsigned char*) 0x25;
volatile unsigned char* portDDRB = (volatile unsigned char*) 0x24;
volatile byte state=0;
volatile short i=0;

// define variables for the motors
const int pinOn = 6;
const int pinCW_L = 8;
const int pinCC_L = 7;
const int pinCW_R = 4;
const int pinCC_R = 5;
const int pinSpeed_L = 10;    // left speed reference
const int pinSpeed_R = 9;     // right speed reference

unsigned long pulse_time = 0;
void setup(){
    *portDDRB |= 0x20;
    pinMode(COMPARITOR, INPUT);
    Serial.begin(9600);
    pinMode(13, OUTPUT);
    pinMode(pinCW_L,OUTPUT);
    pinMode(pinCC_L,OUTPUT);
    pinMode(pinCW_R,OUTPUT);
    pinMode(pinCC_R,OUTPUT);
    digitalWrite(pinCW_L,LOW);
    digitalWrite(pinCC_L,LOW);
    digitalWrite(pinCW_R,LOW);
    digitalWrite(pinCC_R,LOW);
    analogWrite(pinSpeed_L,(DEFAULT_SPEED));
    analogWrite(pinSpeed_R,(MAX_SPEED-40));
    Serial.println("Welcome");
}

void stop(){
    //Serial.println("S   0   0   0");
    if(digitalRead(pinCC_L)==1) 
      digitalWrite(pinCC_L,LOW); 
    if(digitalRead(pinCC_R)==1) 
      digitalWrite(pinCC_R,LOW); 
    if(digitalRead(pinCW_L)==1) 
      digitalWrite(pinCW_L,LOW); 
    if(digitalRead(pinCW_R)==1) 
      digitalWrite(pinCW_R,LOW); 
}
void forward(){
    //Serial.println("0   0   0   F");
    Serial.println(" F");
    stop();
    digitalWrite(pinCC_L,HIGH);
    delay(10);
    digitalWrite(pinCC_R,HIGH);
    delay(100);
}
void left(){
    //Serial.println("0   L   0   0");
    Serial.println(" L");
    //stop();
    digitalWrite(pinCW_L,LOW); 
    digitalWrite(pinCC_R,LOW); 
    digitalWrite(pinCW_L,HIGH); 
    delay(10);
    digitalWrite(pinCC_R,HIGH);
    delay(100);
}
void right(){
    //Serial.println("0   0   R   0");
    Serial.println(" R");
    //stop();
    digitalWrite(pinCC_L,LOW); 
    digitalWrite(pinCW_R,LOW); 
    digitalWrite(pinCC_L,HIGH); 
    delay(10);
    digitalWrite(pinCW_R,HIGH); 
    delay(100);
}
void loop(){
//  digitalWrite(13,HIGH);
  if(digitalRead(COMPARITOR)){
    delay(7);
    i=0;
    if(digitalRead(COMPARITOR)){
      pulse_time = pulseIn(COMPARITOR, HIGH);
      pulse_time += DEBOUNCE;
      //Serial.print(pulse_time);
      if((pulse_time > DEBOUNCE)  && (pulse_time < BOUNDA)){
        stop();
        Serial.println(" S");
        delay(10);
      }
      if((pulse_time > BOUNDA)    && (pulse_time < BOUNDB)){
        left();
      }
      if((pulse_time > BOUNDB)    && (pulse_time < BOUNDC)){
        right();
      }
      if(pulse_time > BOUNDC){
        forward();
      }
    }
  }
  while(digitalRead(COMPARITOR)==0){
    i++;
    if(i=10000) stop();
  }

  // tests basic wheel functionality
  /*
  forward();
  delay(200);
  right();
  delay(200);
  left();
  delay(200);
  stop();
  Serial.println("S");
  delay(200);
  */
}
