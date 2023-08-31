#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "ThingSpeak.h"

// SSID i Lozinka
#define WIFI_SSID "Millenium Cafe"
#define WIFI_PASSWORD "pitajBrunE1304"

#define ZEROING_VALUE 171 //podesiti sa vrijednosti koju poprimi kada je u cistoj vodi

WiFiClient client;

unsigned long myChannelNumber = 2;
const char * myWriteAPIKey = "7YJZ5JAYY279FXM8";

float temperature;
float voltage;
float angle;

//Pin signala temperature
#define ONE_WIRE_BUS 14

//Pin signala napona
#define VoltageIn A0

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);

  if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }

  pinMode(ONE_WIRE_BUS, INPUT);
  sensors.begin();
  
  while (!Serial){
    delay(10);
  }   // wait for serial port to open!
  if(!bno.begin())
  {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
}

void loop() {

  //Citanje nagiba
  sensors_event_t event;
  bno.getEvent(&event);
  angle = event.orientation.z;
  angle = angle + ZEROING_VALUE; 
  angle = abs(angle);

  //Citanje napona
  int value = analogRead(A0);
  voltage = value * (3.3/1023) * 2;

  //Citanje temperature
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  //Slanje na thingspeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, voltage);
  ThingSpeak.setField(3, angle);
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(15000);
}