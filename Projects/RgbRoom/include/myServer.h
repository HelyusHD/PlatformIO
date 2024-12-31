#ifndef SERVER
#define SERVER

#include <iostream>
#include <string>
#include <Arduino.h>
#include <WiFi.h>
#include "networkConfig.h"
#include <HTTPClient.h>
#include <ESPmDNS.h>  // Für mDNS
#include <esp_wifi.h>
#include <WebServer.h>

#include "html_begin.pp" // header from this repo
#include "webpage.html"    // your HTML file
#include "html_end.pp"   // header from this repo

	class EspServer {
	private:
		//esp network
		const std::string networkPassword;
		const std::string networkSSID;
		const std::string dns;
		//home network
		const std::string homeNetworkPassword;
		const std::string homeNetworkSSID;

		WiFiServer server;

		String lastColor[2] = {"000000","000000"};
		std::vector<IPAddress>  connectedIpAdresses;
		int connectedClientsCount = 0;

	public:
		EspServer(std::string password, std::string ssid, std::string dnsName, std::string homePassword, std::string homeSsid)
			: networkPassword(password), networkSSID(ssid), dns(dnsName), homeNetworkPassword(homePassword), homeNetworkSSID(homeSsid), server(80) {}

		// Method to display network information
		void showNetworkInfo() {
			std::cout << "IP Address: " << dns << std::endl;
			std::cout << "Network SSID: " << networkSSID << std::endl;
			std::cout << "Network Password: " << networkPassword << std::endl;
		}

		void networkingSetup() {
			std::cout << "Connecting to WLAN " << homeNetworkSSID << std::endl;
			WiFi.begin(homeNetworkSSID.c_str(), homeNetworkPassword.c_str());

			while (WiFi.status() != WL_CONNECTED) {
				delay(1000);
				std::cout << ".";
				std::cout.flush();
			}
			std::cout << "\nWLAN connected!\nStation-IP (IP in home network):\n" << WiFi.localIP() << std::endl;

			// mDNS starten (Client wird über client01.local angesprochen)
			if (MDNS.begin(dns.c_str())) {
				std::cout << "mDNS gestartet mit Name: " << dns << std::endl;
			} else {
				std::cout <<"Fehler beim Starten von mDNS" << std::endl;
			}

			// Access Point Mode (Eigenes Netzwerk erstellen)
			WiFi.softAP(networkSSID.c_str(), networkPassword.c_str());
			Serial.print("AP-IP: ");
			Serial.println(WiFi.softAPIP()); // Ausgabe der IP im Access Point-Modus

			server.begin();
			std::cout << "Web server started!\n" << std::endl;
		}

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

		// Hexadezimal in Dezimal (R, G, B) umwandeln
		void StringToRgb(String colorHex, uint8_t* rgb){
			rgb[0] = strtol(colorHex.substring(0, 2).c_str(), nullptr, 16);
			rgb[1] = strtol(colorHex.substring(2, 4).c_str(), nullptr, 16);
			rgb[2] = strtol(colorHex.substring(4, 6).c_str(), nullptr, 16);
		}

		void setRgbColor(String colorHex, String ip){
			Serial.println("Farbwert erhalten: " + colorHex+"sending to"+ip);
			uint8_t rgb[3];
			StringToRgb(colorHex, rgb);
			uint8_t r = rgb[0];
			uint8_t g = rgb[1];
			uint8_t b = rgb[2];

			Serial.printf("RGB-Werte: R=%d, G=%d, B=%d\n", r, g, b);
			const String request = "/RgbColor?r=" + String(r) + "&g=" + String(g) + "&b=" + String(b);
			sendHtml(request, ip);
		}

		void handleSetRgbColor(){}
		void handleSendPulse(){}

		void handleClient(){
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
						sendHtml("/turnOff", CLIENT_01);
						sendHtml("/turnOff", CLIENT_02);

					} else if (request.indexOf("/setRgbColor") != -1) {
						// RGB-Farbe aus der Anfrage extrahieren
						int colorIndex = request.indexOf("color=") + 6;
						String colorHex = request.substring(colorIndex, colorIndex + 6);

						int idIndex = request.indexOf("id=") + 3;
						int id = request.substring(idIndex, idIndex + 1).toInt();


						if (id==1){setRgbColor(colorHex, CLIENT_01); lastColor[0] = colorHex;}
						if (id==2){setRgbColor(colorHex, CLIENT_02); lastColor[1] = colorHex;}
					} else if (request.indexOf("/sendPulse") != -1){
						sendHtml("/sendPulse",CLIENT_01);
						sendHtml("/sendPulse",CLIENT_02);
					}
					
					// sending first connection informations to client
					if (request.indexOf("/firstConnection") != -1){
						int idIndex = request.indexOf("id=") + 3;
						int id = request.substring(idIndex, idIndex + 1).toInt();
						uint8_t rgb[3];

						StringToRgb(lastColor[id-1], rgb);
						uint8_t r = rgb[0]; uint8_t g = rgb[1]; uint8_t b = rgb[2];
						String jsonResponse = "	{\"r\":\"" + String(r) + "\", \"g\":\"" + String(g) + "\", \"b\":\"" + String(b) + "\"}";
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type: application/json");
						client.println();
						client.println(jsonResponse);
						Serial.println("send lastColor: " + jsonResponse);
						client.stop();
					} else {
						
						String html = html_page;
						// update last color
						std::cout << html_page << std::endl;

						// HTML-Antwort senden
						client.println(html);
						client.stop();
						Serial.println("Client getrennt.");
					}
				}
			}
		}
	};

#endif
