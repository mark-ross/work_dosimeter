//create a timer for time worked
uint32_t time_worked = 142000000;
int th = (((time_worked/1000)/60)/60);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(th);
  delay(1000);
}
