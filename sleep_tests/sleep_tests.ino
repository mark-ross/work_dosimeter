#include <Sleep_n0m1.h>

//----------------------//
// Sleep settings!
Sleep sleep;
int sleepy_time = 4000;
//----------------------//

uint32_t time_worked = 0;
bool broken = false;

void setup(){
  pinMode(1,INPUT);
  pinMode(0,OUTPUT);
  pinMode(9,OUTPUT);

  Serial.begin(9600);
}

void wakeup(){
  //detachInterrupt(0);
  delay(100);
  Serial.begin(9600);
  Serial.println("Wake up!");
  broken = true;
}

void loop(){
  if(broken == false){
    attachInterrupt(0, wakeup, HIGH);
    
    sleep.pwrDownMode();
    sleep.sleepDelay(sleepy_time);
    
    Serial.println("I've woken from 8 seconds time. . .");
    Serial.println("Going back to sleep now. . .\n");
    time_worked += 4000;
    Serial.println(time_worked);
    delay(100);
  } else {
    digitalWrite(9,HIGH);
  }
}

