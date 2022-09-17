#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <PubSubClient.h>

int sensorPin = 0;
double calcTemperature;

// WiFi parameters to be configured
const char* ssid = "SSID"; // Write here your router's username
const char* password = "PASSWORD"; // Write here your router's password

// MQTT Broker
const char *mqtt_broker = "192.168.178.30";
const char *topic1 = "esp8266/raw";
const char *topic2 = "esp8266/average";
const char *topic3 = "esp8266/temp";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  // Set  serial baud to 9600;
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
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the MQTT broker\n", client_id.c_str());
      if (client.connect(client_id.c_str())) {
          Serial.println("MQTT broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  }

void loop() {
  client.loop();

  // read analog value from defined sensor pin;
  int readingSensor = analogRead(sensorPin);

  // calculate the average from 10 readings and a 300ms delay between;
  int readingAverage = 0;
   for (int i=0; i < 10; i++) {
   readingAverage = readingAverage + analogRead(A0);
   Serial.println(readingAverage);
   delay(300);
   }
   readingAverage = readingAverage/10;
  Serial.println();
  Serial.println("-----------------------");

  // transform the calculated avarage into the temperature, based on a linear function;
  calcTemperature = ((readingAverage-422.25)/14.5);
  Serial.println(calcTemperature);
  Serial.println();
  Serial.println("-----------------------");

  // send messages to MQTT;
  char msg[30];
  char val[10];
  dtostrf(readingSensor, 8, 5, val);
  sprintf(msg, "Raw: %s", val);
  client.publish(topic1, msg);

  char msgAverage[30];
  char valAverage[10];
  dtostrf(readingAverage, 8, 5, valAverage);
  sprintf(msgAverage, "Durchschnitt: %s", valAverage);
  client.publish(topic2, msgAverage);

  char msgTemperature[30];
  char valTemperature[10];
  dtostrf(calcTemperature, 8, 1, valTemperature);
  sprintf(msgTemperature, "Temperature: %s", valTemperature);
  client.publish(topic3, msgTemperature);
  Serial.println();
  Serial.println("----------!!!!!!!!!!!!!!----------");
  Serial.println("FINISHED!");
  Serial.println("----------!!!!!!!!!!!!!!----------");
  Serial.println();
  delay(5000);
}
