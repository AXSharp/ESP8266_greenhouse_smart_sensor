#include <Arduino.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char *ssid = "ssid";
const char *passw = "passw";
int ledPin = D2;
int buttonPin = D7;
int buttonState = 0;
float tem, hum;

DHT sensor(D3, DHT22);
ESP8266WebServer server(8080);

DynamicJsonDocument sensorData(1024);

float getTemperature();
float getHumidity();

void handleSensorData()
{
  sensorData["temperature"] = getTemperature();
  sensorData["humidity"] = getHumidity();

  String response;
  serializeJson(sensorData, response);

  server.send(200, "application/json", response);
}

void setup()
{
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(buttonPin, INPUT);
  sensor.begin();
  WiFi.begin(ssid, passw);
  Serial.println(ssid);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(ledPin, HIGH);

  server.on("/get_sensor_data", HTTP_GET, handleSensorData);
  server.begin();
}

void loop()
{

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      WiFi.disconnect();
    }
    digitalWrite(ledPin, LOW);
    WiFi.begin(ssid, passw);
    Serial.println(ssid);
    Serial.print("Connecting");

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    Serial.println();
    Serial.print("Local IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(ledPin, HIGH);
  }

  server.handleClient();
  Serial.println(String(getTemperature()) + "/" + String(getHumidity()));
  delay(2000);
}

float getTemperature()
{
  return sensor.readTemperature();
}

float getHumidity()
{
  return sensor.readHumidity();
}
