#include <Arduino.h>

void stop();
void write_speed();
void default_speed();
void setup();
void loop();
#line 1 "src/sketch.ino"
// Goal: to make the robot follow the light!
#define MAX 200
// adjust these values so by default, the robot travels (mostly) straight
#define DEFAULT_SPEED_R 110
#define DEFAULT_SPEED_L 70
#define CC_R  2
#define SPD_R 3
#define CW_R  4
#define CC_L  5
#define SPD_L 6
#define CW_L  7

//sensor pins

const uint8_t pinCC_R    = 2;
const uint8_t pinSpeed_R = 3;
const uint8_t pinCW_R    = 4;
const uint8_t pinCC_L    = 5;
const uint8_t pinSpeed_L = 6;
const uint8_t pinCW_L    = 7;

const uint8_t pinSense_A = 8; 
const uint8_t pinSense_B = 9; 
const uint8_t pinSense_C = 10; // Center
const uint8_t pinSense_D = 11; 
const uint8_t pinSense_E = 12; 
//volatile bool A=1,B=1,C=1,D=1,E=1;
volatile uint8_t speedR = DEFAULT_SPEED_R;
volatile uint8_t speedL = DEFAULT_SPEED_L;
//uint8_t last;

struct packed_struct{
  uint8_t A:1;
  uint8_t B:1;
  uint8_t C:1;
  uint8_t D:1;
  uint8_t E:1;
} p;


void stop(){
  PORTD &= ~((1<<CC_L)|(1<<CC_R)|(1<<CW_L)|(1<<CW_R));
  PORTB &= ~(1<<5);
}

void write_speed(){
  if(speedL<MAX) analogWrite(SPD_L, speedL);
  else if(speedL>MAX) analogWrite(SPD_L, MAX);

  if(speedR<MAX) analogWrite(SPD_R, speedR);
  else if(speedR>MAX) analogWrite(SPD_R, MAX);
}

void default_speed(){
  speedR=DEFAULT_SPEED_R;
  speedL=DEFAULT_SPEED_L;
  write_speed();
}

void setup(){
  Serial.begin(9600);
  Serial.println("Willkommen!");
  DDRD  =   0B11111100;
  PORTD = 0B00000000;
  analogWrite(SPD_L, speedL);
  analogWrite(SPD_R, speedR);
  stop(); // look before you roll
}

void loop(){
  // read directly from the pins
  p.A = (PINB & 0B00000001);
  p.B = (PINB & 0B00000010)>>1;
  p.C = (PINB & 0B0000100)>>2;
  p.D = (PINB & 0B0001000)>>3;
  p.E = (PINB & 0B0010000)>>4;
  if(!(p.A | p.B | p.C | p.D | p.E)) PORTB |= (1<<5);
  if(!p.A & p.B & p.C & p.D & p.E) {    // Far Left
    PORTD &=~((1<<CC_L)|(1<<CC_R));
    PORTD |= ((1<<CW_L)|(1<<CW_R));
    default_speed();
    //PORTD &=~((1<<CC_L)|(1<<CC_R)|(1<<CW_L));
    //PORTD |= (1<<CW_R);
    //Serial.println(PORTD,BIN);
    //Serial.println("A");
  }
  if(p.A & !p.B & p.C & p.D & p.E) {   // Left
    //PORTD &= ~((1<<CC_L)|(1<<CC_R)|(1<<CW_R));
    //PORTD |=  (1<<CW_L);
    PORTD &=~((1<<CW_L)|(1<<CC_R));
    PORTD |= ((1<<CC_L)|(1<<CW_R));
    speedL-=10;
    speedR++;
    analogWrite(SPD_R,speedR);
    analogWrite(SPD_L,speedL);
    //Serial.println("B");
  }
  if(p.A & p.B & !p.C & p.D & p.E) {   // Center
    //PORTD = 0B00110000;
    PORTD &=~((1<<CW_L)|(1<<CC_R));
    PORTD |= ((1<<CC_L)|(1<<CW_R));
    default_speed();
    //Serial.println("C");
  }
  if(!p.B & !p.C & !p.D) {          // Too Close
    //PORTD = 0B10000100;
    //PORTD &=~((1<<CC_L)|(1<<CW_R));
    //PORTD |= ((1<<CW_L)|(1<<CC_R));
  }
  if(p.A & p.B & p.C & !p.D & p.E) {   // Right
    //PORTD &= ~((1<<CC_L)|(1<<CW_L)|(1<<CW_R));
    //PORTD |=  (1<<CC_R);
    PORTD &=~((1<<CW_L)|(1<<CC_R));
    PORTD |= ((1<<CC_L)|(1<<CW_R));
    speedR-=10;
    speedL++;
    write_speed();
    //Serial.println("D");
  }
  if(p.A & p.B & p.C & p.D & !p.E) {  // Far Right
    PORTD &=~((1<<CW_L)|(1<<CW_R));
    PORTD |= ((1<<CC_L)|(1<<CC_R));
    default_speed();
    //Serial.println("E");
  }
  if(p.A & p.B & p.C & p.D & p.E){
    stop();
  }
}
