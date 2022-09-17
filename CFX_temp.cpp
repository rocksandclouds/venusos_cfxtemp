#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <PubSubClient.h>

int sensorPin = 0;
double calcTemperature;

// WiFi parameters to be configured
const char* ssid = "SSID"; // Write here your router's username
const char* password = "PASSWORD"; // Write here your router's password

// MQTT Broker
const char *mqtt_broker = "192.168.178.20";
const int mqtt_port = 1883;
const char *topic_verbose_raw = "esp8266/raw";
const char *topic_verbose_average = "esp8266/average";
const char *topic_write_value = "W/VRM-ID/temperature/3/Temperature";

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
  }

void loop() {
  client.loop();

  // read analog value from defined sensor pin;
  int readingSensor = analogRead(sensorPin);

  // calculate the average from 30 readings and a 2000ms delay between;
  int readingAverage = 0;
   for (int i=0; i < 30; i++) {
   readingAverage = readingAverage + analogRead(A0);
   delay(2000);
   }
   readingAverage = readingAverage/30;

  // transform the calculated avarage into the temperature, based on a linear function;
  calcTemperature = ((readingAverage-422.25)/14.5);

// send messages to MQTT;

/*
  char msg[30];
  char val[10];
  dtostrf(readingSensor, 8, 5, val);
  sprintf(msg, "Raw: %s", val);
  client.publish(topic_verbose_raw, msg);

  char msgAverage[30];
  char valAverage[10];
  dtostrf(readingAverage, 8, 5, valAverage);
  sprintf(msgAverage, "Average: %s", valAverage);
  client.publish(topic_verbose_average, msgAverage);
*/

  //connecting to the mqtt broker
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

  char msgTemperature[30];
  char valTemperature[10];
  dtostrf(calcTemperature, 8, 1, valTemperature);
  sprintf(msgTemperature, "{\"value\": %s }", valTemperature);
  client.publish(topic_write_value, msgTemperature);
  Serial.print("Temperature: ");
  Serial.println(calcTemperature);
}
