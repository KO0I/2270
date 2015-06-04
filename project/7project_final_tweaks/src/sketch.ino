/*******************************************************************
 * LAB 6: Final Project
 * Patrick Harrington & William Fischer
 * 12/12/13
 * ****************************************************************/
#define MAX 200
#define ROTTIME 1300 //determined by experimentation
// adjust these values so by default, the robot travels (mostly) straight
#define DEFAULT_SPEED_R 110
#define DEFAULT_SPEED_L 70
#define CC_R  2
#define SPD_R 3
#define CW_R  4
#define CC_L  5
#define SPD_L 6
#define CW_L  7
#define MEANING_OF_LIFE  42

//sensor pins if using digitalWrite
/*
const uint8_t pinCC_R    = 2;
const uint8_t pinSpeed_R = 3;
const uint8_t pinCW_R    = 4;
const uint8_t pinCC_L    = 5;
const uint8_t pinSpeed_L = 6;
const uint8_t pinCW_L    = 7;
*/
uint16_t hold_time = 0;


//const uint8_t pinSense_A = 8; 
//const uint8_t pinSense_B = 9; 
//const uint8_t pinSense_C = 10; // Center
//const uint8_t pinSense_D = 11; 
//const uint8_t pinSense_E = 12; 
//volatile bool A=1,B=1,C=1,D=1,E=1;
volatile uint8_t speedR = DEFAULT_SPEED_R;
volatile uint8_t speedL = DEFAULT_SPEED_L;
//uint8_t last;
// Instead of having each sensor value be 8 bits long, this struct defines a
// bit-field where the sensor values are each given a width of 1 bit. 
// Inspired by http://www.cs.cf.ac.uk/Dave/C/node13.html
struct packed_struct{
  uint8_t A:1;
  uint8_t B:1; // left
  uint8_t C:1;
  uint8_t D:1; // right
  uint8_t E:1; 
  uint8_t F:1;
  uint8_t fetched:1;
} p;
// p for "packed" - a single letter was used 
// to make the later programming a little more
// terse.

void stop(){
  PORTD &= ~((1<<CC_L)|(1<<CC_R)|(1<<CW_L)|(1<<CW_R));
  delay(50);
}

void write_speed(){
  if(speedL<MAX) analogWrite(SPD_L, speedL);
  else if(speedL>MAX) analogWrite(SPD_L, MAX);

  if(speedR<MAX) analogWrite(SPD_R, speedR);
  else if(speedR>MAX) analogWrite(SPD_R, MAX);
}

void bumperHit(){
  if(p.F & !p.fetched){
  stop();
  p.F =     (PINC & 0B0000001);
  PORTB |= (1<<5);
  PORTD &=~((1<<CC_L)|(1<<CW_R));
  PORTD |= ((1<<CW_L)|(1<<CC_R));
  hold_time=0;
  delay(800);
  PORTB &= ~(1<<5);
  stop();
  //delay(100);
  p.F =     (PINC & 0B0000001);
  //check again, and, if there's still something attached, turn around
  if(p.F){
    PORTB |=(1<<5);
    p.fetched =1;
    //slow down speed when rotating
    /*
    speedR=70;
    speedL=30;
    analogWrite(SPD_L, speedL);
    analogWrite(SPD_R, speedR);
    */
    PORTD &=~((1<<CC_L)|(1<<CC_R));
    PORTD |= ((1<<CW_L)|(1<<CW_R));
    delay(ROTTIME);
    stop();
    //set it normal afterwards
    /*
    speedR=110;
    speedL=70;
    analogWrite(SPD_L, speedL);
    analogWrite(SPD_R, speedR);
    */
    PORTD &=~((1<<CC_L)|(1<<CC_R));
    }
  }
}

void setup(){
  p.fetched=0;
  //Serial.begin(9600);
  //Serial.println("Welcome!");
  DDRD  =   0B11111100;
  PORTD = 0B00000000;
  DDRB  |=   (1<<5);
  PORTB &=  ~(1<<5);  // pull low
  PORTC &=  ~(1<<0);  // pull low
  analogWrite(SPD_L, speedL);
  analogWrite(SPD_R, speedR);
  stop(); // look before you roll
}

void loop(){
  // read directly from the pins
  // I use the binary numbers because it was a little easier to relate to the
  // locations of the pins on the Arduino.
  // this was done as an attempt to read pin values faster than digitalRead();
  p.A =     PINB;
  p.B =     (PINB)>>1;
  p.C =     (PINB)>>2;
  p.D =     (PINB)>>3;
  p.E =     (PINB)>>4;
  p.F =     PINC;
  //Serial.println(sizeof(p)); returns a single byte as expected
  // Note: for A-E, negative logic is used. This is because the IR detectors 
  // go LOW when they see a valid signal. p.F is the bumper signal
  if(p.F & !p.fetched){                 // if the bumper is knocked or held
    bumperHit();
  }
  else if(p.fetched & !p.F){            // if ball is "dropped"
    PORTB &= ~(1<<5);
    p.fetched=0;
  }
  else if(!p.A  & p.C & p.D & p.E) {    // Far Left or A
    //note: it's doesn't matter if sensor B is also seeing something or not
    PORTD &=~((1<<CC_L)|(1<<CC_R));
    PORTD |= ((1<<CW_L)|(1<<CW_R));
    //Serial.println("A");
  }
  else if(p.A & !p.B & p.D & p.E) {   // Left or B
    PORTD &= ~((1<<CC_L)|(1<<CC_R)|(1<<CW_L));
    PORTD |=  (1<<CW_R);
    //Serial.println("B");
  }
  else if((p.A & p.B & !p.C & p.D & p.E)|(p.A & !p.B & !p.C & !p.D & p.E)) {   // Center or C
    // if both B and D are seeing a signal, the light is just close; keep moving ahead!
    PORTD &=~((1<<CW_L)|(1<<CC_R));
    PORTD |= ((1<<CC_L)|(1<<CW_R));
    //Serial.println("C");
  }
  else if(p.A & p.B & !p.D & p.E) {   // Right or D
    PORTD &= ~((1<<CW_L)|(1<<CC_R)|(1<<CW_R));
    PORTD |=  (1<<CC_L);
    //Serial.println("D");
  }
  else if(p.A & p.B & p.C & !p.E) {  // Far Right or E
    //note: it's doesn't matter if sensor D is also seeing something or not
    PORTD &=~((1<<CW_L)|(1<<CW_R));
    PORTD |= ((1<<CC_L)|(1<<CC_R));
    //Serial.println("E");
  }
  else if(p.A & p.B & p.C & p.D & p.E){ // nothing seen; nothing done 
    stop();
  }
}
