#ifndef SERVER
#define SERVER

#include <iostream>
#include <string>
#include <Arduino.h>
#include <WiFi.h>
#include "networkConfig.h"
#include <HTTPClient.h>
#include <ESPmDNS.h>  // Für mDNS
#include "webpage.h"
#include <esp_wifi.h>

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

        String lastColor = "000000";
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

        void handleSetRgbColor(){}
        void handleSendPulse(){}

        void handleClient(){
            // Webserver nur im Heimnetzwerk bedienen
            if (WiFi.status() == WL_CONNECTED) {

                int numDevices = WiFi.softAPgetStationNum();
                if(numDevices != connectedClientsCount){
                    std::cout << numDevices << " != " << connectedClientsCount << std::endl;
                    remeberNewDevice();
                }

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

                    } else if (request.indexOf("/setRgbColor?color=") != -1) {
                        // RGB-Farbe aus der Anfrage extrahieren
                        int colorIndex = request.indexOf("color=") + 6;
                        String colorHex = request.substring(colorIndex, colorIndex + 6);

                        lastColor = colorHex;

                        setRgbColor(colorHex, CLIENT_01);
                        setRgbColor(colorHex, CLIENT_02);
                    } else if (request.indexOf("/sendPulse") != -1){
                        sendHtml("/sendPulse",CLIENT_01);
                        sendHtml("/sendPulse",CLIENT_02);
                    }
                    // update last color
                    String html = webpage;
                    html.replace("$LASTCOLOR", "#" + String(lastColor)); // Inject lastColor into the webpage


                    // HTML-Antwort senden
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println(html);
                    client.stop();
                    Serial.println("Client getrennt.");
                }
            }
        }
        void remeberNewDevice() {
            // Get detailed station info
            wifi_sta_list_t stationList;
            tcpip_adapter_sta_list_t adapterStationList;

            esp_wifi_ap_get_sta_list(&stationList); // Get station list
            tcpip_adapter_get_sta_list(&stationList, &adapterStationList); // Map to adapter station list

            
            // Update the count of connected clients
            connectedClientsCount = 0;

            // Loop through each connected station
            for (int i = 0; i < adapterStationList.num; i++) {
                tcpip_adapter_sta_info_t station = adapterStationList.sta[i];
                IPAddress deviceIP = IPAddress((uint8_t*)&station.ip.addr);

                if (deviceIP.toString() == "0.0.0.0") {
                    Serial.printf("Device %d has no valid IP yet. Skipping.\n", i + 1);
                    continue;
                }

                connectedClientsCount++;
                
                // comparing new IP to known IPs
                bool isNewDevice = true;
                for (IPAddress ip : connectedIpAdresses){
                    if (ip == deviceIP){
                        isNewDevice = false;
                        break;
                    }
                }

                if(isNewDevice){
                    Serial.printf("New device %d: MAC: %02X:%02X:%02X:%02X:%02X:%02X, IP: %s\n",
                    i + 1,
                    station.mac[0], station.mac[1], station.mac[2],
                    station.mac[3], station.mac[4], station.mac[5],
                    deviceIP.toString());
                    setRgbColor(lastColor, deviceIP.toString());
                }
            }
            // Clear the vector to reset the list of known IP addresses
            connectedIpAdresses.clear();

            // Save all currently connected device IPs into the vector
            for (int i = 0; i < adapterStationList.num; i++) {
                tcpip_adapter_sta_info_t station = adapterStationList.sta[i];
                IPAddress deviceIP = IPAddress((uint8_t*)&station.ip.addr);
                connectedIpAdresses.push_back(deviceIP); // Add the device IP to the vector
            }
        }
    };

#endif
