#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <PubSubClient.h>

int sensorPin = 0;
int AverageSamples = 10;

// WiFi parameters to be configured
const char* ssid = "@home"; // Write here your router's username
const char* password = "abc"; // Write here your router's password

// MQTT Broker
const char *mqtt_broker = "192.168.178.30";
const char *topic = "esp8266/test";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);



void setup() {
  // Set software serial baud to 115200;
  Serial.begin(9600);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str())) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  //  subscribe
  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void loop() {
  client.loop();
  int SensorReading = analogRead(sensorPin);
  Serial.print(sensorPin);

  int average = 0;
  for (int i = 0; i < AverageSamples; ++i) {
   average += SensorReading / AverageSamples;
  delay(20);
  }

  char msg[30];
  char val[10];
  dtostrf(SensorReading, 8, 5, val);
  sprintf(msg, "Wert: %s", val);
  client.publish(topic, msg);

  char msgAverage[30];
  char valAverage[10];
  dtostrf(SensorReading, 8, 5, valAverage);
  sprintf(msg, "Durchschnitt: %s", valAverage);
  client.publish(topic, msgAverage);
  delay(5000);
}