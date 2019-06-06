// defines pins numbers
const int trigPin  = 2;  //D4
const int echoPin  = 0;  //D3
const int redLed   = 5;  //D1
const int greenLed = 4;  //D2

// defines variables
const int globalDistance = 20;
long      duration       = 0;
int       distance       = 0;

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.begin(9600);
}
void loop() {
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  distance= duration*0.034/2;
  
  Serial.print("Distância: ");
  Serial.println(distance);
  
  if(distance < (globalDistance - 5)){
    Serial.println("DETECTADO!!!!!");
    turnOnRed();
  } else {
    turnOnGreen();
  }
  
  delay(500);
}

void turnOnRed() {
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, LOW);
}

void turnOnGreen() {
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, HIGH);
}
