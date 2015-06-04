/**************************************************
Counts the time that the comparitor is high
and prints it out to Serial
**************************************************/
#define COMPARITOR 12
#define DEBOUNCE  7000
volatile unsigned long pulse_time = 0;

//volatile unsigned char * portB = (volatile unsigned char *) 0x25;
//volatile unsigned char * portDDRB  = (volatile unsigned char *) 0x24;
//unsigned char news = FALSE;
volatile unsigned long lowest=20*DEBOUNCE,highest=0;

void setup(){
    pinMode(COMPARITOR, INPUT);
//    *portDDRB |= 0x20;
    Serial.begin(9600);
    Serial.println("Welcome");
}

void loop(){
  if(!digitalRead(COMPARITOR)){
//    *portB &= 0xDF;
  pulse_time = 0;
  }else{
  delay((DEBOUNCE/1000)); //debounce - don't care about tiny pulses
  // we chose delay based on effectiveness at ignoring
  // spikes
  if(digitalRead(COMPARITOR)) pulse_time = pulseIn(COMPARITOR, HIGH);
    if(pulse_time != 0){
      // if dividing by 1000, then we are using ms
      //Serial.println(((pulse_time+DEBOUNCE)));
      pulse_time+=DEBOUNCE;
      if(pulse_time < lowest){
        lowest=pulse_time;
        }
      if(pulse_time > highest){
        highest=pulse_time;
        Serial.print("Lo: ");
        Serial.print(lowest);
        Serial.print(" Hi: ");
        Serial.println(highest);
        }
      //Serial.println("us");
      }
//  *portB |= 0x20;
  }
}
