#include <Arduino.h>

int degrees2enc(int degrees);
int mm2enc(int mm);
int speed2pwm(int percent);
void count_Left();
void count_Right();
void stopall();
void setup();
void turnL(int pulses);
void turnR(int pulses);
void turn(int pulses,int speed);
void execute(int distance, int speed, int type);
void compensate();
void loop();
#line 1 "src/sketch.ino"
// Constants
#define FULLROT 768 - 19
#define DISTMM 2*(610  -178)
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
volatile short enc_count_L = 0;
volatile short enc_count_R = 0;
volatile int diff = 0;
volatile int speed_diff = 0;
volatile  int x = 0;
volatile  int y = 0;
volatile  int z = 0;
volatile int v = 0;
volatile int mode=0;
volatile int j=0;

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
  stopall(); // all wheels should be immobile at start
}

void turnL(int pulses){
  pulses-=10; //if using 50 for "brake" speed on left wheel
  analogWrite(pinSpeed_L,L_DEFAULT);
  digitalWrite(pinCW_L,HIGH);
  do {
    if(enc_count_L > pulses/2)
      analogWrite(pinSpeed_L,50);
  } while(enc_count_L < pulses);
  digitalWrite(pinCW_L,LOW);
}
void turnR(int pulses){
  pulses-=14; //if using 50 for "brake" speed on left wheel
  analogWrite(pinSpeed_R,R_DEFAULT);
  digitalWrite(pinCW_R,HIGH);
  do {
    if(enc_count_R > pulses/2)
      analogWrite(pinSpeed_R,50);
  } while(enc_count_R < pulses);
  digitalWrite(pinCW_R,LOW);
}
void turn(int pulses,int speed){
  enc_count_R = 0;
  enc_count_L = 0;
  digitalWrite(pinCC_L,HIGH);
  digitalWrite(pinCC_R,HIGH);
  while((enc_count_L < pulses) && (enc_count_R < pulses) ){
  analogWrite(pinSpeed_L, speed);
  analogWrite(pinSpeed_R, speed);
  }
  digitalWrite(pinCC_L,LOW);
  digitalWrite(pinCC_R,LOW);
}

//-----------------------------------------------------------------
//  Begin Moving
//-----------------------------------------------------------------

void execute(int distance, int speed, int type){ //distance in pulses, speed in x/255
  //remember, from tests, the best speed for CC is around 80, (near MAX)
  //both wheels have to reach their value before we stop
  int pR=pinCC_L;
  int pL=pinCW_R;
  int NpR=pinCW_L;
  int NpL=pinCC_R;
  if(type == 1){
    //run straight
    pR = pinCC_R;
    pL = pinCW_L;
    NpR = pinCW_R;
    NpL = pinCC_L;
    }
  if(type == 2){
    //run backwards
    pR = pinCW_R;
    pL = pinCC_L;
    NpR = pinCC_R;
    NpL = pinCW_L;
    }
  if(type == 3){
    //rotate cw
    pR = pinCC_R;
    pL = pinCC_L;
    NpR = pinCW_R;
    NpL = pinCW_L;
    }
  if(type == 4){
    //rotate cc
    pR = pinCW_R;
    pL = pinCW_L;
    NpR = pinCC_R;
    NpL = pinCC_L;
    }
  speed_R = 0;
  speed_L = 0;
  v = 0;
  speed_L = speed;
  int speed_R = speed+3;
  if(speed < 70) speed_R+=(0.1 *speed);
  analogWrite(pinSpeed_R, speed_R);
  analogWrite(pinSpeed_L, speed_L);
  digitalWrite(pR, HIGH);
  digitalWrite(pL, HIGH);
  Serial.println("GO!----------------------------------------------");
  delay(150);
  enc_count_L = 0;
  enc_count_R = 0;
  while((enc_count_L < distance) || (enc_count_R < distance)){
    if((enc_count_L > (0.5*distance)) &&(enc_count_R > (0.5*distance))){
      speed_L--;
      v = 1;
      analogWrite(pinSpeed_L,(speed_L));
      //analogWrite(pinSpeed_R,(speed_R));
      //Serial.println("nearly there!");
      }
   //if((enc_count_L > 0.8*distance) && (enc_count_R > 0.8*distance)){
   //   analogWrite(pinSpeed_L, speed_L-30);
   //   analogWrite(pinSpeed_R, speed_R-30);}
    diff = enc_count_L - enc_count_R;
    if(diff ==0) mode=0;
    if(diff > 0){
      if(diff > 15){
        mode=2; x=1;
        if(diff > 50){
          mode=3; y=1;
          if(diff > 45){
          mode=4; z=1;}else{ mode=3; z=0;}
          }else{mode=2;y=0;z=0;}
        }else{mode=1; x=0;y=0;z=0;}
      speed_R+=(1+x+y+z);
      //speed_L-=(1+x+y+z);
      if(speed_R>MAX) speed_R=MAX;
      if(speed_R>MAX) speed_R=MAX;
      //if(speed_L<MIN) speed_L=MIN;
      //if(speed_L<MIN) speed_L=MIN;
      analogWrite(pinSpeed_R, speed_R);
      //analogWrite(pinSpeed_L, speed_L);
      //delay(100); // reduce or remove for faster action
    }
    
    if(diff < 0){
      if(diff < -15){
        mode=-1; x=1;
        if(diff < -35){
          mode=-3; y=2;
          if(diff < -50){
          mode=-4; z=1;}else{mode=-3; z=0;}
          }else{mode=-2; y=0;z=0;}
        }else{mode=-1;x=0;y=0;z=0;}
      speed_R-=(1+x+y+z-v);
      //speed_L+=(1+x+y+z);
      if(speed_R<MIN) speed_R=MIN;
      if(speed_R>MAX) speed_R=MAX;
      //if(speed_L<MIN) speed_L=MIN;
      //if(speed_L>MAX) speed_L=MAX;
      analogWrite(pinSpeed_R, speed_R);
      //analogWrite(pinSpeed_L, speed_L);
      
      //delay(100);
    }
   /* 
    speed_diff=speed_R-speed_L;
    if(speed_diff>60){
      speed_R-=3;
 //     speed_L++;
      if(speed_R<MIN) speed_R=MIN;
 //     analogWrite(pinSpeed_R,speed_R);
    }
    if(speed_diff<-40){
      speed_R+=3;
 //     speed_L--;
      if(speed_R>MAX) speed_R=MAX;
      analogWrite(pinSpeed_R,speed_R);
 //     analogWrite(pinSpeed_L,speed_L);
    }*/

    //  report
    Serial.print("diff: ");
    Serial.print(diff);
    Serial.print("       speed_R: ");
    Serial.print(speed_R);
    Serial.print("       speed_L: ");
    Serial.print(speed_L);
    Serial.print("   mode:    ");
    Serial.println(mode);

  }//end of while loop
  Serial.print("enc_count_L: ");
  Serial.print(enc_count_L);
  Serial.print("      enc_count_R: ");
  Serial.println(enc_count_R);
  digitalWrite(pR,LOW);
  digitalWrite(pL,LOW);
  analogWrite(pinSpeed_R,MAX);
  analogWrite(pinSpeed_L,MAX);
  digitalWrite(NpR,HIGH);
  digitalWrite(NpL,HIGH);
  delay(95);
  digitalWrite(NpR,LOW);
  digitalWrite(NpL,LOW);
  delay(1);
  digitalWrite(pR,HIGH);
  digitalWrite(pL,HIGH);
  delay(10);
  digitalWrite(pR,LOW);
  digitalWrite(pL,LOW);
  
  delay(100);
  Serial.print("post_enc_count_L: ");
  Serial.print(enc_count_L);
  Serial.print("      post_enc_count_R: ");
  Serial.println(enc_count_R);
  diff = 0;
}

void compensate(){
  j+=13;
  }
void loop(){
do {
  } while (digitalRead(pinOn) == LOW);
  digitalWrite(13,HIGH);
  delay(1000);
  //turnL(FULLROT);
  //turnR(FULLROT);
  //turn(10000,speed2pwm(50));
  //execute(2*FULLROT,speed2pwm(15),1);
  //delay(600);
  execute((DISTMM+9+j),speed2pwm(53),1);
  delay(400);
  execute(FULLROT-16,speed2pwm(35),3); //CC rotation
  delay(400);
  execute((DISTMM-30-j),speed2pwm(53),1);
  delay(400);
  execute(FULLROT-12,speed2pwm(35),4); // CW rotation
  digitalWrite(13,LOW);
  delay(400);
  compensate();
  Serial.println(j);
  }
