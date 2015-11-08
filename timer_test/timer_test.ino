#include <elapsedMillis.h>

/* Create the timers needed for the program */
elapsedMillis t1; //button press timer
elapsedMillis t2; //loop timer

int f = 1000; //1 second
int g = 1500; //1.5 seconds

void setup() {
  
  // set timers to 0
  t1 = 0;
  t2 = 0;

  // set the Serial 
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(t1 >= f) {
    Serial.println("Timer1!");
    t1 -= f;
  } else if(t2 >= g){
    Serial.println("Timer2!");
    t2 -= g;
  }
}
