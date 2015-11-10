int vib = 10;

void setup() {
  // put your setup code here, to run once:
  pinMode(vib, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(vib,HIGH);
  delay(100);
  digitalWrite(vib,LOW);
  delay(100);
}
