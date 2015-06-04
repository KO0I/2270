// Constants
#define FULLROT 768
#define DISTMM 610
#define R_DEFAULT 50
#define L_DEFAULT 50
#define MAX 230   // this value should be the maximum only for the
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
volatile int delta_diff = 0;
//volatile int rel_count_L = 0;
//volatile int rel_count_R = 0;
volatile  int x = 0;
volatile  int y = 0;
volatile  int z = 0;

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
int check_limits(int speed);
// ISRs
void count_Left(){
  enc_count_L++;
//  rel_count_L++;
}
void count_Right(){
  enc_count_R++;
//  rel_count_R++;
}

//------------------------------------------------------------------
//  Begin Moving
//------------------------------------------------------------------


void execute(int distance, int speed, int type){ //distance in pulses, speed in x/255
  //remember, from tests, the best speed for CC is around 80, (near MAX)
  //both wheels have to reach their value before we stop
  int pR=pinCC_L;
  int pL=pinCW_R;
  if(type == 1){
    //run straight
    pR = pinCC_R;
    pL = pinCW_L;
    }
  if(type == 2){
    //run backwards
    pR = pinCW_R;
    pL = pinCC_L;
    }
  if(type == 3){
    //rotate cw
    pR = pinCC_R;
    pL = pinCC_L;
    }
  if(type == 4){
    //rotate cc
    pR = pinCW_R;
    pL = pinCW_L;
    }
  speed_R = 0;
  speed_L = 0;
  int i = 0;
  speed_L = speed;
  int speed_R = speed-3;
  //Serial.println(speed);
  // under 70% duty cycle, the left wheel is approx 1.1 times faster
  if(speed < 70) speed_R+=(0.1 *speed);
  analogWrite(pinSpeed_R, speed_R);
  analogWrite(pinSpeed_L, speed_L);
  digitalWrite(pR, HIGH);
  digitalWrite(pL, HIGH);
  delay(800);
  enc_count_L = 0;
  enc_count_R = 0;
//  rel_count_L = 0;
//  rel_count_R = 0;
//  delay(10); // consider removing or reducing for faster reaction
  // should change to '&&' once confident about feedback quality
  while((enc_count_L < distance) && (enc_count_R < distance)){
  // slow start to deal with delay problems?
  /*
    if((enc_count_L > distance/10) && (enc_count_R > distance/10)){
      if((enc_count_L > 0.2*distance) && (enc_count_R > 0.2*distance)){
        if((enc_count_L > 0.3*distance) && (enc_count_R > 0.3*distance)){
          if((enc_count_L > 0.35*distance) && (enc_count_R > 0.35*distance)){
            if((enc_count_L > 0.4*distance) && (enc_count_R > 0.4*distance)){ 
              analogWrite(pinSpeed_L, speed_L);}
            analogWrite(pinSpeed_L, (0.8*speed_L));}
          analogWrite(pinSpeed_L, (0.6*speed_L));}
        analogWrite(pinSpeed_L, (0.3*speed_L));}
      analogWrite(pinSpeed_L, 20);} */
    // the braking is done by slowing down the left wheel 
    // to a fraction of the given speed
    // the curve is at 50% complete -> 90% of given speed
    //              at 60% complete -> 80% of given speed
    //              at 70% complete -> 60% of given speed
    //              at 80% complete -> 30% of given speed
    //              at 90% complete -> set to 8% pwm (20/255)
    // wait for both wheels to get above the required threshold
    if((enc_count_L > 0.8*distance) && (enc_count_R > 0.8*distance)){
//      if((enc_count_L > 0.5*distance) || (enc_count_R > 0.5*distance)){
    //    if((enc_count_L > 0.6*distance) || (enc_count_R > 0.6*distance)){
    //      if((enc_count_L > 0.7*distance) || (enc_count_R > 0.7*distance)){
    //        if((enc_count_L > 0.8*distance) || (enc_count_R > 0.8*distance)){ 
    //          analogWrite(pinSpeed_L, 20);}
    //        analogWrite(pinSpeed_L, (0.3*speed_L));}
    //      analogWrite(pinSpeed_L, (0.6*speed_L));}
    //    Serial.println("end is near!");
    //    analogWrite(pinSpeed_L, 0);}
      analogWrite(pinSpeed_L, speed_L-z);
      analogWrite(pinSpeed_R, speed_R-z);}
    diff = enc_count_L - enc_count_R;
    //rel_count_L = 0;
    //rel_count_R = 0;
        if(diff > 0){ //less than two encoder pulse difference is irrelevant
      if(diff > 20){
        if(diff > 30){
          if(diff > 50){ z=3;}else z=0;
        y=2;}else y=0;
      x=1;}else z=0;
      speed_R+=(2+x+y+z);
      //speed_L-=2;
      if(speed_R>MAX) speed_R=MAX;
      if(speed_R<MIN) speed_R=MIN;
      //if(speed_L<MINPERCENT) speed_L=MINPERCENT;
      analogWrite(pinSpeed_R, speed_R);
      //analogWrite(pinSpeed_L, speed_L);
      delay(80); // reduce or remove for faster action
    }
    
    if(diff < 0){
      if(diff < -20){
        if(diff < -30){
          if(diff < -50) z=3;
          else z=0;
        y=2;}else y=0;
      x=1;}else x=0;
      speed_R-=(1+x+y+z);
      //speed_L+=2;
      if(speed_R<MIN) speed_R=MIN;
      if(speed_R>MAX) speed_R=MAX;
      //if(speed_L<MAXPERCENT) speed_L=MAXPERCENT;
      analogWrite(pinSpeed_R, speed_R);
      //analogWrite(pinSpeed_L, speed_L);
      
      delay(100);
    }
    //  report
    Serial.print("diff: ");
    Serial.print(diff);
    delta_diff = 0;
    Serial.print("       speed_R: ");
    Serial.print(speed_R);
    Serial.print("       speed_L: ");
    Serial.println(speed_L);


  }//end of while loop
    
  digitalWrite(pR,LOW);
  digitalWrite(pL,LOW);
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


void loop(){
do {
  } while (digitalRead(pinOn) == LOW);
  digitalWrite(13,HIGH);
  //turnL(FULLROT);
  //turnR(FULLROT);
  //turn(10000,speed2pwm(50));
  execute(5*FULLROT,speed2pwm(60),1);
  delay(1000);
  execute(5*FULLROT,speed2pwm(60),2);
  digitalWrite(13,LOW);
  delay(1000);
  }
