#include <Arduino.h>

void setup();
void ISR_countLeft();
void ISR_countRight();
void startCC();
void startCW();
void stopCC();
void stopCW();
void runStraight(int mm);
void turn(int turn180);
void brake_CWrotation();
void loop();
#line 1 "src/sketch.ino"
/******************************************************************
* 
******************************************************************/

// Distance to be covered in units of mm
#define DISTMM 610
// Number of encoder pulses required to make a full rotation
// (derived from experimentation)
#define FULLROT 880
// This is the default pwm (out of 255)
#define DEFAULT_SPEED 95
#define SPEED_BOOST   0// to help overcome encoder error
// MAX is the pwm where the wheel reaches its highest value;
// and where a higher pwm will not increase wheel speed further
#define MAX 160
// MIN is the lowest pwm such that the wheel appreciably rotates
#define MIN 0

// define pins
const int pinCW_Right = 4;   // 
const int pinCC_Right = 5;   // 
const int pinON = 6;         // connect pin 6 to ON/OFF switch, active HIGH
const int pinCW_Left = 7;    // connect pin 7 to clock-wise PMOS gate
const int pinCC_Left = 8;    // connect pin 8 to counter-clock-wise PMOS gate
const int pinSpeedRight =9;      // connect pin 9 to right speed reference
const int pinSpeedLeft =10;      // connect pin 10 to left speed reference
int speed_Right = DEFAULT_SPEED;
int speed_Left  = DEFAULT_SPEED;
volatile int enc_countLeft = 0;
volatile int enc_countRight = 0;
volatile int diff = 0;
volatile int bc = 1;         // braking coefficient


// setup pins and initial values
void setup() {
  attachInterrupt(0, ISR_countLeft,RISING);
  attachInterrupt(1, ISR_countRight,RISING);
  Serial.begin(9600);
  // switch and its indicator
  pinMode(pinON,INPUT);
  pinMode(13,OUTPUT);             // on-board LED
  // pins for PMOS gate control, two directions for each wheel
  pinMode(pinCW_Right,OUTPUT);
  pinMode(pinCC_Right,OUTPUT);
  pinMode(pinCW_Left,OUTPUT);
  pinMode(pinCC_Left,OUTPUT);
  // speed control
  pinMode(pinSpeedRight,OUTPUT);
  pinMode(pinSpeedLeft,OUTPUT);
  analogWrite(pinSpeedRight,speed_Right); // set speed reference, duty-cycle
  analogWrite(pinSpeedLeft,speed_Left); // set speed reference, duty-cycle
  // start with both wheels off
  digitalWrite(pinCW_Left,LOW);   // stop clockwise
  digitalWrite(pinCC_Left,LOW);   // stop counter-clockwise
  //pinMode(3,INPUT);
}
//Interrupts for each wheel
void ISR_countLeft(){
  enc_countLeft++;
  }

void ISR_countRight(){
  enc_countRight++;
  }

// These four functions start and stop the set of wheels for both CC and CW
void startCC(){
    digitalWrite(pinCC_Right,HIGH);
    digitalWrite(pinCC_Left,HIGH);
    analogWrite(pinSpeedLeft,DEFAULT_SPEED);
}
void startCW(){
    digitalWrite(pinCW_Right,HIGH);
    digitalWrite(pinCW_Left,HIGH);
}
/******************************************************************
* the stop functions here could use a little improvement for
* better accuracy. See Erikson's course notes for details.
* The gist of it is w(t) = ( Sqrt[2 TMAX/J]*Sqrt[thetaf-theta[t]] )
* when near the final destination, we need to apply a braking
* torque by starting the wheels in the opposite direction for 
* a short time.
*******************************************************************/
void stopCC(){
    digitalWrite(pinCC_Right,LOW);
    digitalWrite(pinCC_Left,LOW);
}
void stopCW(){
    digitalWrite(pinCW_Right,LOW);
    digitalWrite(pinCW_Left,LOW);
}
void runStraight(int mm){
  int b = 0;
  diff=0;
  enc_countRight = 0;
  enc_countLeft = 0;
// each encoder pulse represents 0.53mm
// so, every 2 pulses is approximately 1mm
// so if given 1000mm (1m), we need to count up 2000 pulses
  int count = (2*mm);
// while both wheels are below the count, drive the wheels
  startCC();
  // give the reference wheel a small boost!
  analogWrite(pinSpeedLeft,(DEFAULT_SPEED+SPEED_BOOST));
  while( (enc_countRight <= count) && (enc_countLeft <= count) ){
    //if(enc_countLeft>=60){
      // after a short while, bring the wheel speed down
      //analogWrite(pinSpeedLeft,(DEFAULT_SPEED));
      // if master wheel is 90% near the target, halve its speed
      if(enc_countLeft >= 1100){
        analogWrite(pinSpeedLeft,(DEFAULT_SPEED/2));
        b=1;
        }
    //}
    /******************************************************************** 
    * find difference in wheel speed
    * each encoder is counting, but usually at different rates
    * if diff is NEGATIVE, the right wheel is going FASTER than the left
    * if diff is POSITIVE, the right wheel is going SLOWER that the left
    ********************************************************************/
    diff = (enc_countLeft - enc_countRight);
    //Serial.println(diff);
   // if the right wheel lags, speed it up!
    if(diff > 0){
      speed_Right += (6-b);
      //speed_Right= speed_Right + diff*1/10;
      //Serial.println(speed_Right);
      if(speed_Right > MAX) speed_Right = MAX;
      analogWrite(pinSpeedRight,speed_Right);
      // if we overshoot MAX, just force the wheel speed to MAX
    }
    // if the right wheel is going too fast, slow it down
    if(diff < 0){
      speed_Right -= (3-b);
    //  speed_Right= speed_Right + diff*1/10;
      if(speed_Right < MIN) speed_Right = MIN;
      //Serial.println(speed_Right);
      analogWrite(pinSpeedRight,speed_Right);
      //Serial.print("Speed_right: ");
      //Serial.println(speed_Right);
      //Serial.print("pin: ");
      //Serial.println(digitalRead(pinSpeedRight));
      // if we somehow go below MIN, set to MIN
    }
  } // end of while loop

  // cheaty way of implimenting a "brake"
  Serial.println("STOP!");
  stopCC();
  startCW();
  delay(7);
  stopCW();
  // report values... if you care
  //Serial.print("SpeedRight: ");
  //Serial.println(speed_Right);
  //Serial.print("SpeedLeft: ");
  //Serial.println(speed_Left);
  //Serial.print("diff: ");
  //Serial.println(diff);
  //if(diff<0){
  //  Serial.println("The robot drifts initially to the right...");}
  //if(diff>0){
  //  Serial.println("The robot drifts initially to the left...");}
  //  if(diff==0){
  //  Serial.println("Straight as an arrow!");}
  //Serial.print(enc_countLeft);
  //Serial.print(", ");
  //Serial.println(enc_countRight);
  //delay(2000);
  // reset counters before the start over
  diff=0;
  enc_countRight = 0;
  enc_countLeft = 0;
}

// turns the robot around. 
void turn(int turn180){
  int b = 0;
  diff=0;
  enc_countRight = 0;
  enc_countLeft = 0;
// each encoder pulse represents 0.53mm
// so, every 2 pulses is approximately 1mm
// so if given 1000mm (1m), we need to count up 2000 pulses
//int count = (2*mm);
int count = FULLROT*turn180;
// while both wheels are below the count, drive the wheels
  digitalWrite(pinCW_Right,HIGH);
  digitalWrite(pinCC_Left,HIGH);
  analogWrite(pinSpeedLeft,(DEFAULT_SPEED-20));
  while( (enc_countRight <= count)){// && (enc_countLeft <= count) ){
    //if(enc_countLeft>=60){
      // after a short while, bring the wheel speed down
      //analogWrite(pinSpeedLeft,(DEFAULT_SPEED));
    // a slowdown procedure
    if((enc_countLeft >= (0.7*count)) && (enc_countRight >= (0.7*count))){
      analogWrite(pinSpeedLeft,(DEFAULT_SPEED/2));
      b = 1;
      if((enc_countRight >= (0.8*count))&&(enc_countLeft >=
      (0.8*count))){
        analogWrite(pinSpeedLeft,(DEFAULT_SPEED/3));
        b = 3;
        if((enc_countLeft >= (count-2))&&(enc_countRight >=
        (count-2)))brake_CWrotation();
      //}
      }
    }
    // start the wheels!
    //analogWrite(pinSpeedRight,255);
    /******************************************************************** 
    * find difference in wheel speed
    * each encoder is counting, but usually at different rates
    * if diff is NEGATIVE, the right wheel is going FASTER than the left
    * if diff is POSITIVE, the right wheel is going SLOWER that the left
    ********************************************************************/
    diff = (enc_countLeft - enc_countRight);
    //Serial.println(diff);
   // if the right wheel lags, speed it up!
    if(diff > 0){
    speed_Right += (5-b);
    //speed_Right+=diff*0.2;
      if(speed_Right > MAX) speed_Right = MAX;
      analogWrite(pinSpeedRight,speed_Right);
      // if we overshoot MAX, just force the wheel speed to MAX
    }
    // if the right wheel is going too fast, slow it down
    if(diff < 0){
      speed_Right -= (4-b);
      //speed_Right+=diff*.2;
      if(speed_Right < MIN) speed_Right = MIN;
      analogWrite(pinSpeedRight,speed_Right);
      // if we somehow go below MIN, set to MIN
    }
  } // end of while loop
  digitalWrite(pinCW_Right,LOW);
  digitalWrite(pinCC_Left,LOW);
  diff=0;
  //  Serial.print("L: ");
  //  Serial.println(enc_countLeft);
  //  Serial.print("R: ");
  //  Serial.println(enc_countRight);
  enc_countRight = 0;
  enc_countLeft = 0;
}

void brake_CWrotation(){
  // kills rotation in the robot when it does its about-face
  digitalWrite(pinCW_Right,LOW);
  digitalWrite(pinCC_Left,LOW);
  //lock the wheels to freeze the robot
  digitalWrite(pinCC_Right,HIGH);
  digitalWrite(pinCW_Left,HIGH);
  delay(3);
  digitalWrite(pinCC_Right,LOW);
  digitalWrite(pinCW_Left,LOW);
  }

void loop() {
    digitalWrite(13,LOW);                     // turn LED off
    do {} while (digitalRead(pinON) == LOW);  // wait for ON switch
    runStraight(DISTMM);
    // the little delays are important unless braking is correctly implimented
    delay(650);
    turn(1);
    delay(650);
    //runStraight(DISTMM);
    //delay(650);
    //turn(2);
  }
