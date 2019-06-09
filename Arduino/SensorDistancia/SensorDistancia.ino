#include <NewPing.h>

#define TRIGGER_PIN  11
#define ECHO_PIN     10
#define MAX_DISTANCE 200
#define DISTANCIA 36
#define LED_VERMELHO 8
#define LED_VERDE 9

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  delay(50);

  int distanciaAtual = sonar.ping_cm();

  // Print inicial para definir DISTANCIA
  // Serial.println(distanciaAtual);

  if(distanciaAtual < (DISTANCIA-5)){ // 5 é a margem de erro entre uma leitura e outra
    delay(1000);
    digitalWrite(LED_VERMELHO, HIGH);
    digitalWrite(LED_VERDE, LOW);
    Serial.println("OCUPADO");
  } else {
    delay(1000);
    digitalWrite(LED_VERMELHO, LOW);
    digitalWrite(LED_VERDE, HIGH);
    Serial.println("LIVRE");
  }
}
