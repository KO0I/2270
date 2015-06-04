/*************************************************
Turns on an LED if the Arduino detects a pulse
from the COMPARITOR. If the comparitor has no
output, the LED is also low (off).
*************************************************/

#define COMPARITOR 12
//#define LED_PIN 13

// for LED
volatile unsigned char * portB = (volatile unsigned char *) 0x25;
volatile unsigned char * portDDRB  = (volatile unsigned char *) 0x24;

void setup(){
  //pinMode(LED_PIN,OUTPUT);
  *portDDRB |= 0x20;
  pinMode(COMPARITOR, INPUT);
  // let's attach the receiver chain output to pin 2
}

void loop(){
  if(!digitalRead(COMPARITOR)){
    //digitalWrite(LED_PIN,LOW);
    *portB &= 0xDF;
  }else{
    //digitalWrite(LED_PIN,HIGH);
    *portB |= 0x20;
  }
}
