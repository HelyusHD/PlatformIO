#include <Arduino.h>
#include <WiFi.h>
#include "networkConfig.h"
#include <HTTPClient.h>
#include <ESPmDNS.h>  // Für mDNS
#include "webpage.h"

// WLAN-Credentials für das bestehende Netzwerk (Heimnetzwerk)
const char* ssid_STA = HOME_NETWORK_SSID;
const char* password_STA = HOME_NETWORK_PASSWORD;

// WLAN-Credentials für das ESP32-eigene Netzwerk
const char* ssid_AP = ESP_NETWORK_SSID;
const char* password_AP = ESP_NETWORK_PASSWORD;

String dnsName = "esp"; // real DNS domain will be dnsName + ".local"
String tableLampController = CLIENT_01;
String bedLampController = CLIENT_02;

// Globale Variablen
WiFiServer server(80); // Webserver auf Port 80

void sendHtml(String request, String ip){
  Serial.println("trying to send request to "+ip);
  HTTPClient http;
  String serverPath = "http://" + ip + request;

  http.begin(serverPath); // Anfrage an Client01 senden
  int httpResponseCode = http.GET(); // GET-Anfrage senden

  if (httpResponseCode > 0) {
    Serial.print("Antwort vom Client: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Fehler bei der Anfrage: ");
    Serial.println(httpResponseCode);
  }

  http.end(); // Verbindung schließen
}

// Funktion, die den Pin 5 des Client01-ESP32 steuert
void turnOff(String ip) {
  const String request = "/turnOff";
  sendHtml(request, ip);
}

void setRgbColor(String colorHex, String ip){
  Serial.println("Farbwert erhalten: " + colorHex+"sending to"+ip);

  // Hexadezimal in Dezimal (R, G, B) umwandeln
  uint8_t r = strtol(colorHex.substring(0, 2).c_str(), nullptr, 16);
  uint8_t g = strtol(colorHex.substring(2, 4).c_str(), nullptr, 16);
  uint8_t b = strtol(colorHex.substring(4, 6).c_str(), nullptr, 16);
  Serial.printf("RGB-Werte: R=%d, G=%d, B=%d\n", r, g, b);

  const String request = "/RgbColor?r=" + String(r) + "&g=" + String(g) + "&b=" + String(b);
  sendHtml(request, ip);
}



void setup() {
  Serial.begin(115200);

  // Verbindung mit WLAN herstellen (Station Mode)
  Serial.println("Verbinde mit WLAN...");
  WiFi.begin(ssid_STA, password_STA);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWLAN verbunden!");
  Serial.print("Station-IP: ");
  Serial.println(WiFi.localIP()); // Ausgabe der IP im Heimnetzwerk

  // mDNS starten (Client wird über client01.local angesprochen)
  if (MDNS.begin(dnsName)) {
    Serial.println("mDNS gestartet mit Name: " + dnsName);
  } else {
    Serial.println("Fehler beim Starten von mDNS");
  }

  // Access Point Mode (Eigenes Netzwerk erstellen)
  WiFi.softAP(ssid_AP, password_AP);
  Serial.print("AP-IP: ");
  Serial.println(WiFi.softAPIP()); // Ausgabe der IP im Access Point-Modus

  // Webserver starten
  server.begin();
}

void loop() {
  // Webserver nur im Heimnetzwerk bedienen
  if (WiFi.status() == WL_CONNECTED) {
    // Client abfragen
    WiFiClient client = server.available();
    if (client) {
      Serial.println("Neuer Client verbunden!");

      // Anfrage vom Client einlesen
      String request = client.readStringUntil('\r');
      Serial.println("Anfrage: " + request);
      client.flush();

      // Anfragen verarbeiten

      if (request.indexOf("/turnOff") != -1) {
        turnOff(tableLampController);
        turnOff(bedLampController);
      } else if (request.indexOf("/setRgbColor?color=") != -1) {
        // RGB-Farbe aus der Anfrage extrahieren
        int colorIndex = request.indexOf("color=") + 6;
        String colorHex = request.substring(colorIndex, colorIndex + 6);
        setRgbColor(colorHex, tableLampController);
        setRgbColor(colorHex, bedLampController);
      } else if (request.indexOf("/sendPulse") != -1){
        sendHtml("/sendPulse",tableLampController);
        sendHtml("/sendPulse",bedLampController);
      }

      // HTML-Antwort senden
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println(webpage);
      client.stop();
      Serial.println("Client getrennt.");
    }
  }
}
