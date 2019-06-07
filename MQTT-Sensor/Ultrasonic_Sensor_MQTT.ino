#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

// maximum received message length 
#define MAX_MSG_LEN (128)

// Wifi configuration
const char* ssid     = "RoboRasp";
const char* password = "RoboRasp";

// MQTT Configuration

const char *serverHostname = "192.168.1.101";
const char *topicMessage   = "sector1/message";
const char *topicParked    = "sector1/parked";

WiFiClient espClient;
PubSubClient client(espClient);

// defines pins numbers
const int trigPin  = 2;  //D4
const int echoPin  = 0;  //D3
const int redLed   = 5;  //D1
const int greenLed = 4;  //D2

// defines variables
const int globalDistance = 20;
long      duration       = 0;
int       distance       = 0;
int       lastDistance   = 0;
bool      parked         = false;

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.begin(9600);

  connectWifi();
  // connect to MQTT server  
  client.setServer(serverHostname, 1883);
  client.setCallback(callback);
}
void loop() {
  if (!client.connected()) {
      connectMQTT();
  }
  
  client.loop();
  
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
  distance = duration*0.034/2;
  
  Serial.print("Distância: ");
  Serial.println(distance);
  
  if(distance < (globalDistance - 5)){
    Serial.println("DETECTADO!!!!!");
    if(parked == false){
      client.publish(topicParked, "here");
      Serial.println("a car just parked, sending to topic...");
      client.subscribe(topicMessage);
    }
    lastDistance = distance;
  } else if (lastDistance != distance) {
    client.publish(topicParked, "not");
  }
  
  delay(5000);
}

void callback(char *msgTopic, byte *msgPayload, unsigned int msgLength) {
  // copy payload to a static string
  static char message[MAX_MSG_LEN+1];
  if (msgLength > MAX_MSG_LEN) {
    msgLength = MAX_MSG_LEN;
  }
  strncpy(message, (char *)msgPayload, msgLength);
  message[msgLength] = '\0';
  
  Serial.printf("topic %s, message received: %s\n", topicMessage, message);

  // decode message
  if (strcmp(message, "ok") == 0) {
    parked = true;
  } else if (strcmp(message, "green") == 0) {
    turnOnGreen();
  } else if (strcmp(message, "red") == 0) {
    turnOnRed();
    parked = false;
  } else {
    Serial.println("Aguardando resposta...");
  }
}

void turnOnRed() {
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, LOW);
}

void turnOnGreen() {
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, HIGH);
}

void connectWifi() {
  delay(10);
  // Connecting to a WiFi network
  Serial.printf("\nConnecting to %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected on IP address ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  // Wait until we're connected
  while (!client.connected()) {
    String clientId = "Spot01";
    Serial.printf("MQTT connecting as client %s...\n", clientId.c_str());
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
      // Once connected, publish an announcement...
      client.publish(topicMessage, "hello from Spot01");
      Serial.println("I'm Spot1, waiting for something to happen...");
      turnOnGreen();
      // ... and resubscribe
      client.subscribe(topicMessage);
    } else {
      Serial.printf("MQTT failed, state %s, retrying...\n", client.state());
      // Wait before retrying
      delay(2500);
    }
  }
}
