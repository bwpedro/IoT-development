#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

#define MAX_MSG_LEN (128)

// Wifi configuration
const char* ssid     = "";
const char* password = "";

// MQTT Configuration
const char *serverHostname = "";
const char *topicMessage   = "spot1/message";
const char *topicParked    = "spot1/parked";
WiFiClient espClient;
PubSubClient client(espClient);

// defines pins numbers
const int trigPin  = 2;  //D4
const int echoPin  = 0;  //D3
const int redLed   = 5;  //D1
const int greenLed = 4;  //D2

// defines variables
int standardDistance = 0;
int curentDistance   = 0;
int lastDistance     = 0;
long duration        = 0;
bool parked          = false;

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

  standardDistance = getDistance();
  Serial.print("Standard distance: ");
  Serial.println(standardDistance);
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  
  client.loop();
  
  currentDistance = getDistance();
  
  Serial.print("Current distance: ");
  Serial.println(currentDistance);
  
  if(currentDistance < (standardDistance - 2)){
    Serial.println("CAR DETECTED!!!!!");
    if(parked == false){
      client.publish(topicParked, "busy");
      Serial.println("message 'here' sended to '%s'\n", topicMessage);
      client.subscribe(topicMessage);
    }
  } else if (lastDistance != currentDistance) {
    client.publish(topicParked, "free");
    Serial.println("message 'not' sended to '%s'\n", topicMessage);
  }

  lastDistance = currentDistance;

  if (currentDistance == standardDistance){
    Serial.println("Waiting for a car ...");
  }
  
  delay(5000);
}

int getDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  currentDistance = duration*0.034/2;
  
  return currentDistance;
}

void callback(char *msgTopic, byte *msgPayload, unsigned int msgLength) {
  
  static char message[MAX_MSG_LEN+1];
  if (msgLength > MAX_MSG_LEN) {
    msgLength = MAX_MSG_LEN;
  }
  strncpy(message, (char *)msgPayload, msgLength);
  message[msgLength] = '\0';
  
  Serial.printf("message '%s' received from '%s'\n", message, topicMessage);
  if (strcmp(message, "ok") == 0) {
    parked = true;
  } else if (strcmp(message, "green") == 0) {
    turnOnGreen();
    parked = false;
  } else if (strcmp(message, "red") == 0) {
    turnOnRed();
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
  
  while (!client.connected()) {

    String clientId = "Spot1";
    Serial.printf("MQTT connecting as client %s...\n", clientId.c_str());

    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");

      String message = clientId + "connected!";
      client.publish(topicMessage, message);
      Serial.println("message %s sended to '%s'\n", message, topicMessage);

      client.subscribe(topicMessage);

    } else {
      Serial.printf("MQTT failed, state %s, retrying...\n", client.state());
      delay(2500);
    }
  }
}
