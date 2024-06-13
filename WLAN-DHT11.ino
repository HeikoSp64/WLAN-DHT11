//V1.2
#include <Wire.h>
#include "SSD1306Wire.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

// DHT
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
DHT dht(15, DHTTYPE);

// Display
SSD1306Wire display(0x3c, SDA, SCL);

// WiFi
const char* ssid     = "Sperling RB";
const char* password = "20sperling10";

// MQTT
const char* mqtt_server = "192.168.0.240"; 
const char* humidity_topic = "humidity-s1";
const char* temperature_topic ="temperature-s1";
const char* mqtt_username ="heiko"; // MQTT username
const char* mqtt_password ="merlin"; // MQTT password
const char* clientID ="ESP32"; // MQTT client ID

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;

// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient);

 

// Custom function to connect to the MQTT broker via WiFi
void connect_MQTT(){
  
  // Connect to MQTT Broker
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed…");
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
 display.init();
 // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());



}

void loop() {
  
  connect_MQTT();
  Serial.setTimeout(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  Serial.print("Humidity:");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Temperature:");
  Serial.print(t);
  Serial.println(" *C");

  // MQTT can only transmit strings
  String hs="Hum:"+String((float)h)+" %";
  String ts="Temp:"+String((float)t)+" C";

  // PUBLISH to the MQTT Broker (topic = Temperature)
  if (client.publish(temperature_topic, String(t).c_str())) {
    Serial.println("Temperature sent!");
  }
  else {
    Serial.println("Temperature failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn’t clash with the client.connect call
    client.publish(temperature_topic, String(t).c_str());
  }

  // PUBLISH to the MQTT Broker (topic = Humidity)
  if (client.publish(humidity_topic, String(h).c_str())) {
    Serial.println("Humidity sent!");
  }
  else {
    Serial.println("Humidity failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn’t clash with the client.connect call
    client.publish(humidity_topic, String(h).c_str());
  }


 client.disconnect();  // disconnect from the MQTT broker
  delay(1000*60);       // print new values after 1 Minute

 //String t1 = String(temp);
 //   String h1 = String(hum);
   
   
   //display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Temperatur");
    display.drawString(0, 17, ts );
    display.drawString(33, 17, "Grad");
    display.drawString(0, 34, "Luftfeuchtigkeit");
    display.drawString(0, 51, hs); 
   
    display.display();

}