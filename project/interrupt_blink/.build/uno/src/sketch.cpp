#include <Arduino.h>

void setup();
void button_on();
void loop();
#line 1 "src/sketch.ino"
#define LED_PIN 13

void setup(){
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    attachInterrupt(0, button_on, RISING);
}

void button_on(){
    digitalWrite(LED_PIN, HIGH);
    delay(500);
}

void loop(){
}
