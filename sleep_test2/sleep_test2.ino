#include <LowPower.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  Serial.println("Wake up from sleep!");
  delay(2000);
}
