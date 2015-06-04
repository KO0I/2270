// Constants

#include <PID_v1.h>

#define FULLROT 768 -12
#define DISTMM 610  -10
#define R_DEFAULT 50
#define L_DEFAULT 50
#define MAX 240   // this value should be the maximum only for the
                  // RIGHT wheel. the left wheel is slower overall
                  // At 100% pwm, the right wheel is 1.04kHz. The 
                  // left is around 960Hz. MAX is set to 90% pwm.
#define MIN 0
#define MAXPERCENT 85
#define MINPERCENT 2

// Global Variables
const int pinOn = 6;
const int pinCW_L = 7;
const int pinCC_L = 8;
const int pinCW_R = 4;
const int pinCC_R = 5;
const int pinSpeed_L = 10;    // left speed reference
const int pinSpeed_R = 9;     // right speed reference
volatile int speed_R;         // pwm% speed of wheels
volatile int speed_L;
volatile int enc_count_L = 0;
volatile int enc_count_R = 0;
volatile int diff = 0;
PID wheel(&(double)enc_count_R,&(double)speed_R,&(double)diff,1,1,1,DIRECT);

// Helper Functions
int degrees2enc(int degrees){
  int enc=2*degrees; // simple approximation
  // There are 360 degrees for ever 768 pulses,
  // This means that for ever 2.13 encoder pulses,
  // the wheel has moved 1 degree
  // for every 8 encoder pulses, we add a single
  // pulse for improved accuracy
  if(degrees % 8){
    int correction = degrees/8;
    enc += correction;
  }
  // example: 32 degrees is 68.2666... enc pulses. The simple
  // approx gives 64 pulses. Since 32 is divisible by 8, when
  // divided by 8 it gives 4, which brings our corrected enc
  // value to 68, which is more than 99% accurate and 6% better
  // than the "simple" approximation
return enc;
}

int mm2enc(int mm){
// 0.53 mm traversed per encoder pulse
// so to go a millimeter, about two encoder pulses needed
return 2*mm;
}

int speed2pwm(int percent){
  return (percent*255)/100;
}
// ISRs
void count_Left(){
  enc_count_L++;
}
void count_Right(){
  enc_count_R++;
}
void stopall(){
  digitalWrite(pinCC_L,LOW);
  digitalWrite(pinCC_R,LOW);
  digitalWrite(pinCW_L,LOW);
  digitalWrite(pinCW_R,LOW);
}
void setup(){
  Serial.begin(9600);
  attachInterrupt(0, count_Left, RISING);
  attachInterrupt(1, count_Right, RISING);
  pinMode(pinCW_L,OUTPUT);
  pinMode(pinCC_L,OUTPUT);
  pinMode(pinCW_R,OUTPUT);
  pinMode(pinCC_R,OUTPUT);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  digitalWrite(pinCW_L,LOW);
  digitalWrite(pinCC_L,LOW);
  digitalWrite(pinCW_R,LOW);
  digitalWrite(pinCC_R,LOW);
  analogWrite(pinSpeed_L,L_DEFAULT);
  analogWrite(pinSpeed_R,R_DEFAULT);
  enc_count_L = 0;
  enc_count_R = 0;
  wheel.SetMode(AUTOMATIC);
  stopall();
}

void loop(){
do {
  } while (digitalRead(pinOn) == LOW);
  digitalWrite(13,HIGH);
  delay(100);
  digitalWrite(pinCW_L,HIGH);
  digitalWrite(pinCW_R,HIGH);
  analogWrite(pinCW_R, 100);
  analogWrite(pinCW_L, 100);
  while((enc_count_L < 610) && (enc_count_R < 610)){
  wheel.Compute();
  analogWrite(pinCW_R, speed_R);
  Serial.print(speed_R);
  Serial.print("        ");
  Serial.println(speed_L);
  }
  Serial.print("Left: ");
  Serial.print(enc_count_L);
  Serial.print("Right: ");
  Serial.println(enc_count_R);
}
