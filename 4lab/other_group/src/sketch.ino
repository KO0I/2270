#define target2feet 1050
#define target180CC 741
#define target180CW 751  //750


const int pinON = 6; // connect pin 6 to ON/OFF switch, active HIGH 

const int pinCC_Left = 7; // connect pin 7 to clock-wise PMOS gate 
const int pinCW_Left = 8; // connect pin 8 to counter-clock-wise PMOS gate 

const int pinSpeed_Left = 9; // connect pin 9 to speed reference 
const int pinSpeed_Right = 10;

const int pinCC_Right = 4;
const int pinCW_Right = 5;

volatile int enc_count_left = 0;
volatile int enc_count_right = 0;

volatile int diff;

volatile int speed_right = 100;
volatile int speed_left = 100;  //max 193  min 73

const int speed_const = 15;
const int speed_center=133;  //133
const int speed_gap=15;
volatile int speed_level;

// setup pins and initial values 

void setup() { 
 attachInterrupt(0, ISR_count_left, RISING);
 attachInterrupt(1, ISR_count_right, RISING);
 
 Serial.begin(9600);
 pinMode(pinON,INPUT); 
 pinMode(pinCW_Left,OUTPUT); 
 pinMode(pinCC_Left,OUTPUT); 
 pinMode(pinSpeed_Left,OUTPUT); 
 pinMode(13,OUTPUT); // on-board LED 
 digitalWrite(pinCW_Left,LOW); 
 digitalWrite(pinCC_Left,LOW);  
 pinMode(pinCW_Right, OUTPUT);
 pinMode(pinCC_Right, OUTPUT);
 pinMode(pinSpeed_Right, OUTPUT);
 digitalWrite(pinCW_Right, LOW);
 digitalWrite(pinCC_Right, LOW);
 } 

void adjustment(){
  diff = enc_count_left - enc_count_right;
  speed_level = diff/ speed_const;
  if(speed_level > 4){
    speed_level =4;
  }
  else if(speed_level < -4){
    speed_level=-4;
  }
  speed_left=speed_center-speed_level*speed_gap;
  speed_right=speed_center+speed_level*speed_gap;
  analogWrite(pinSpeed_Right,speed_right);
  analogWrite(pinSpeed_Left,speed_left);
}
  
  void reset(){
     digitalWrite(pinCW_Left,LOW); 
     digitalWrite(pinCC_Left,LOW); 
     digitalWrite(pinCW_Right, LOW);
     digitalWrite(pinCC_Right, LOW);
     enc_count_left = 0;
     enc_count_right = 0;
  }
  
  void forward2feet(){
    while(target2feet >= enc_count_left ){ //&& target2feet >= enc_count_right){
    digitalWrite(pinCC_Left,HIGH);
    digitalWrite(pinCW_Right,HIGH);
    adjustment();
  }
  }
  
  void robot180CC(){
    while(target180CC >= enc_count_left){ // && target180CC >= enc_count_right){
  digitalWrite(pinCC_Right, HIGH);
  digitalWrite(pinCC_Left, HIGH);
  adjustment();
  }
  }
  
  void robot180CW(){
    while(target180CW >= enc_count_left){ // && target180CW >= enc_count_right){
    digitalWrite(pinCW_Left,HIGH);
    digitalWrite(pinCW_Right,HIGH);
    adjustment();
  }
  }

void loop() { 
  while(digitalRead(pinON)==LOW) reset();
  reset();
   delay(1000);
   reset();
   
   forward2feet();    
   reset();
   delay(250);  //500
   reset();
   
   robot180CC();
   reset();
   delay(250);
   reset();
   
   forward2feet();
   reset();
   delay(250);
   reset();
   
   robot180CW();
   reset();  
   delay(250);
   reset();
   
   Serial.println(enc_count_left);
   Serial.println(enc_count_right);
   Serial.println(pinSpeed_Left);
   Serial.println(pinSpeed_Right);
}

void ISR_count_left(){
  enc_count_left++;
}

void ISR_count_right(){
  enc_count_right++;
}

  /*
  
  if(diff>= 0){
    speed_right = 120;
    speed_left = 80;
  }
  else
 { 
   speed_right = 80;
   speed_left =120;
  
 }
 
  
    diff = enc_count_left - enc_count_right;

  if (diff >= 0){
  speed_right = speed_right + diff*speed_const;
  if(speed_right > 220){
    speed_right = 220;
    
  }
  speed_left = speed_left - diff*speed_const;
  if(speed_left < 90){
    speed_left = 90;
  }
 }
  
  else {
    speed_right = speed_right + diff*speed_const;
    if(speed_right < 90){
      speed_right = 90;
    }
    speed_left = speed_left - diff*speed_const;
    if(speed_left > 220){
      speed_left = 220;
    }
  }
 
 
 
 */
