#include <Arduino.h>
#include <WiFi.h>
#include "networkConfig.h"
#include <WebServer.h>
#include <FastLED.h>
#include "lampControl.h"

#define LED_PIN 5
#define NUM_LEDS 83 // 57 bed + 26 sofa
const char* ipAdress = CLIENT_02; // Use the IP as a string

// creating list that will hold all LEDs in it
CRGB leds[NUM_LEDS];
CRGB background[NUM_LEDS];

PulseManager pulseManager(leds, NUM_LEDS);

int ledMapping[NUM_LEDS];

// WLAN-Credentials für das bestehende Netzwerk (Heimnetzwerk)
const char* ssid_STA = ESP_NETWORK_SSID;
const char* password_STA = ESP_NETWORK_PASSWORD;

IPAddress local_IP; // Declare the IPAddress object
IPAddress gateway(192, 168, 4, 1);    // Gateway (IP des "master")
IPAddress subnet(255, 255, 255, 0);   // Subnetzmaske

void staticRgbColor(CRGB rgbColor){
fill_solid(leds, NUM_LEDS, rgbColor);
fill_solid(background, NUM_LEDS, rgbColor);

  FastLED.show(); // LEDs aktualisieren
}
void ledsOff(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
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

  for(int i=0;i<NUM_LEDS;i++){
    ledMapping[i] = i;
  }
  //storing LEDs in array and assigning pins
  FastLED.addLeds<WS2811, LED_PIN>(leds, NUM_LEDS);
}

void loop() {
  server.handleClient(); // handle server requests from any clients
  pulseManager.updatePulses(ledMapping, background); // Hintergrundfarbe Schwarz
}
