#define FULLROT 768 -12
#define DISTMM 610  -10
#define MAX 190
#define MIN 0
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
volatile int speed_diff = 0;
volatile  int x = 0;
volatile  int y = 0;
volatile  int z = 0;
volatile int mode=0;

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
  enc_count_L = 0;
  enc_count_R = 0;
}
// ISRs
void count_Left(){
  enc_count_L++;
}
void count_Right(){
  enc_count_R++;
}


void speedcontrol(int distance, int speed, int type){
  int pL=pinCW_L;
  int pR=pinCW_R;
  if(type == 1){
    //run straight
    pR=pinCC_R;
    pL=pinCW_L;
  }
  if(type == 2){
    //run backwards
    pR=pinCW_R;
    pL=pinCC_L;
  }
  if(type == 3){
    //rotate cw
    pR=pinCC_R;
    pL=pinCC_L;
  }
  if(type== 4){
    //rotate cc
    pR=pinCW_R;
    pL=pinCW_L;
  }
    analogWrite(pinSpeed_L,speed);
    analogWrite(pinSpeed_R,speed);
    digitalWrite(pinCW_R, HIGH);
    digitalWrite(pinCW_L, HIGH);
    enc_count_L = 0;
    enc_count_R = 0;

while((enc_count_L < distance) || (enc_count_R < distance)){
  diff = enc_count_L - enc_count_R;

  if (diff>=0){
    speed_L = speed_L - diff * 2 /10;
    if ( speed_L < MIN) speed_L = MIN;
    analogWrite(pinSpeed_L, speed_L);

    speed_R = speed_R + diff * 2/10;
    if ( speed_R > MAX) speed_R = MAX;
    analogWrite(pinSpeed_R, speed_R);
    }
  if (diff<0){
    speed_L = speed_L + diff * 2 /10;
    if ( speed_L > MAX) speed_L = MAX;
    analogWrite(pinSpeed_L, speed_L);

    speed_R = speed_R - diff * 2/10;
    if ( speed_R > MAX) speed_R = MAX;
    analogWrite(pinSpeed_R, speed_R);
    }
  digitalWrite(pR, LOW);
  digitalWrite(pL, LOW);
  }
}

void loop(){
  do{}
  while((digitalRead(pinOn) == LOW));
  digitalWrite(13,HIGH);
  speedcontrol(10000,100,1);
  delay(1000);
  digitalWrite(13,LOW);
}
