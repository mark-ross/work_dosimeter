int fsr = 1;
int led = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(fsr,INPUT);
  pinMode(led,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(fsr) == HIGH){
    digitalWrite(led,HIGH);
  }
  else {
    digitalWrite(led,LOW);
  }

}
