int ledVermelho = 9;
int ledVerde = 8;

void setup() {
  pinMode(ledVerde, OUTPUT);
  pinMode(ledVermelho, OUTPUT);
}

void loop() {
  digitalWrite(ledVermelho, HIGH);
  digitalWrite(ledVerde, LOW);
  delay(1000);
  digitalWrite(ledVermelho, LOW);
  digitalWrite(ledVerde, HIGH);
  delay(1000);   
}
