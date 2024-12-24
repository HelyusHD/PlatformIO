#ifndef CLIENT
#define CLIENT

#include <iostream>
#include <string>
#include <vector>
#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>
#include "lampControl.h"

    // Segment for a strip of LEDs
    class stripSegment {
    private:
        CRGB* leds;
        int ledCount;

    public:
        // Constructor
        stripSegment(CRGB* led_array, int start, int end)
            : leds(led_array+start), ledCount(end-start) {}

        // Method to display segment properties
        void showSegmentInfo() {
            std::cout << "Number of LEDs: " << ledCount << std::endl;
        }

        // sets a uniform color
        void setColor(CRGB color){
            fill_solid(leds, ledCount, color);
            FastLED.show();
        }
    };

    class lightStrip {
    private:
        CRGB* leds;
        std::string chipType;
        int ledCount;
        PulseManager pulseManager;
        int* ledMapping;

    public:
        std::vector<stripSegment> segments; // Container for holding multiple strip segments

        // Constructor to initialize the attributes and allocate memory for LEDs
        lightStrip(CRGB* led_array, int led_count, int* map) : leds(led_array), ledCount(led_count), pulseManager(leds, ledCount), ledMapping(map){}

        // Method to display lightStrip properties
        void showStripInfo() {
            std::cout << "Number of LEDs: " << ledCount << std::endl;
        }

        // Method to add a segment
        void addSegment(int start, int end) {
            segments.push_back(stripSegment(leds, start, end));  // Add a new segment to the vector
        }

        // Method to display segment information
        void showSegmentsInfo() {
            for (auto& segment : segments) {
                segment.showSegmentInfo(); // Show each segment's information
            }
        }

        // sets a uniform color
        void setColor(CRGB color){
            fill_solid(leds, ledCount, color);
            FastLED.show();
        }
        void spawnPulse(){
            pulseManager.spawnPulse(CRGB::Red, 1, 0, 5, true); // Roter Impuls, startet bei LED 0, bewegt sich um 1 LED pro Tick, Länge 5 LEDs
        }
        void updatePulse(){
            pulseManager.updatePulses(ledMapping);
        }
    };

    // espClient controller connects to a network and controls lightStrips
    class espClient {
    private:
        const std::string ipAddress;
        const std::string networkPassword;
        const std::string networkSSID;
        WebServer server;

    public:
        std::vector<lightStrip> strips; // Container for holding multiple strip segments

        // Constructor to initialize the attributes
        espClient(std::string ip, std::string password, std::string ssid)
            : ipAddress(ip), networkPassword(password), networkSSID(ssid), server(80) {}

        // Method to display network information
        void showNetworkInfo() {
            std::cout << "IP Address: " << ipAddress << std::endl;
            std::cout << "Network SSID: " << networkSSID << std::endl;
            std::cout << "Network Password: " << networkPassword << std::endl;
        }

        // Method to add a segment
        void addStrip(CRGB* led_array, int led_count, int* map) {
            strips.push_back(lightStrip(led_array, led_count, map));  // Add a new segment to the vector
        }

        // Method to display segment information
        void showStripInfo() {
            for (auto& strips : strips) {
                strips.showStripInfo(); // Show each segment's information
            }
        }

        // turns all lamps off
        void handleOff(){
            for (auto& strips : strips) {
                strips.setColor(CRGB::Black);
            }
        }

        // sets a uniform RGB color
        void handleRgbColor(){
            int r = server.arg("r").toInt();
            int g = server.arg("g").toInt();
            int b = server.arg("b").toInt();
            Serial.printf("got new RGB request %i\t%i\t%i\n", r, g, b);

            CRGB color = CRGB(r,g,b);
            for (auto& strips : strips) {
                strips.setColor(color);
            }
        }

        void handleSendPulse(){
            for (auto& strips : strips) {
                strips.spawnPulse();
            }
        }

        // connects to local esp wifi network
        void networkingSetup() {
            IPAddress local_IP;
            IPAddress gateway(192, 168, 4, 1);
            IPAddress subnet(255, 255, 255, 0); 

            int ip[4];
            sscanf(ipAddress.c_str(), "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
            local_IP = IPAddress(ip[0], ip[1], ip[2], ip[3]);

            std::cout << "Connecting to WLAN " << ipAddress << std::endl;
            WiFi.config(local_IP, gateway, subnet);
            WiFi.begin(networkSSID.c_str(), networkPassword.c_str());

            while (WiFi.status() != WL_CONNECTED) {
                delay(1000);
                std::cout << ".";
                std::cout.flush();
            }
            std::cout << "\nWLAN connected!" << std::endl;

            server.on("/turnOff", HTTP_GET, [this](){this->handleOff(); });
            server.on("/RgbColor", HTTP_GET, [this](){this->handleRgbColor(); });
            server.on("/sendPulse", HTTP_GET, [this](){this->handleSendPulse(); });
            server.begin();
            std::cout << "Web server started!\n" << std::endl;
        }

        // handle server requests from any clients
        void handleClient(){
            server.handleClient();
        }

        void updateAnimations(){
            for (auto& strips : strips) {
                strips.updatePulse();
            }  
        }
    };

#endif
