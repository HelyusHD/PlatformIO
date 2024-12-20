#include <Arduino.h>
#include <WiFi.h>
#include "networkConfig.h"
#include <WebServer.h>
#include <FastLED.h>
#include "lampControl.h"

#define LED_PIN_FL 5
#define LED_PIN_R 4
#define NUM_LEDS_FL 324
#define NUM_LEDS_R 108
const int numLeds = 432;
const char* ipAdress = CLIENT_01; // Use the IP as a string

// creating list that will hold all LEDs in it
CRGB leds[numLeds];
CRGB background[numLeds];

PulseManager pulseManager(leds, numLeds);

int ledMapping[numLeds];

// WLAN-Credentials für das bestehende Netzwerk (Heimnetzwerk)
const char* ssid_STA = ESP_NETWORK_SSID;
const char* password_STA = ESP_NETWORK_PASSWORD;

IPAddress local_IP; // Declare the IPAddress object
IPAddress gateway(192, 168, 4, 1);    // Gateway (IP des "master")
IPAddress subnet(255, 255, 255, 0);   // Subnetzmaske

void staticRgbColor(CRGB rgbColor){
fill_solid(leds, numLeds, rgbColor);
fill_solid(background, numLeds, rgbColor);

  FastLED.show(); // LEDs aktualisieren
}
void ledsOff(){
  fill_solid(leds, numLeds, CRGB::Black);
  FastLED.show();
}


// Webserver auf Port 80
WebServer server(80);

void handleOff() {
  Serial.println("Turning LEDs off.");
  staticRgbColor(CRGB::Black);

  server.send(200, "text/plain", "turned off");
}

void handleRgbColor(){
  int r = server.arg("r").toInt();
  int g = server.arg("g").toInt();
  int b = server.arg("b").toInt();
  Serial.printf("got new RGB request %i\t%i\t%i\n", r, g, b);

  CRGB color = CRGB(r,g,b);
  staticRgbColor(color);
}

void handleSendPulse(){
  pulseManager.spawnPulse(CRGB::Red, 1, 0, 5, true); // Roter Impuls, startet bei LED 0, bewegt sich um 1 LED pro Tick, Länge 5 LEDs
}

void setup() {
  Serial.begin(115200);

  // Parse the string into an IPAddress object
  int ip[4];
  sscanf(ipAdress, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
  local_IP = IPAddress(ip[0], ip[1], ip[2], ip[3]);

  // Verbindung mit WLAN herstellen (Station Mode)
  Serial.println("Verbinde mit WLAN");
  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(ssid_STA, password_STA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWLAN verbunden!");
  Serial.print("Station-IP: ");
  Serial.println(WiFi.localIP());

  // Webserver-Routen
  server.on("/turnOff", HTTP_GET, handleOff);
  server.on("/RgbColor", HTTP_GET, handleRgbColor);
  server.on("/sendPulse", HTTP_GET, handleSendPulse);

  // Webserver starten
  server.begin();

  //storing LEDs in array and assigning pins
  for(int i=0;i<numLeds;i++){
    if(i<NUM_LEDS_R){
      ledMapping[i] = NUM_LEDS_R - i - 1;
    } else {
      ledMapping[i] = i;
    }
  }
  FastLED.addLeds<WS2815, LED_PIN_R, GRB>(leds, 0, NUM_LEDS_R); // Erster Strang
  FastLED.addLeds<WS2815, LED_PIN_FL, GRB>(leds, NUM_LEDS_R, NUM_LEDS_FL); // Zweiter Strang
}

void loop() {
  server.handleClient(); // handle server requests from any clients
  pulseManager.updatePulses(ledMapping, background); // Hintergrundfarbe Schwarz
}
