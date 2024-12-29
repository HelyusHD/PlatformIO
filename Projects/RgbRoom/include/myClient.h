#ifndef CLIENT
#define CLIENT

#include <iostream>
#include <string>
#include <vector>
#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>
#include "myAnimations.h"

	// Segment for a strip of LEDs
	class StripSegment {
	private:
		CRGB* leds;
		CRGB* background;
		int ledCount;

	public:
		// Constructor
		StripSegment(CRGB* led_array, CRGB* background_array, int start, int end)
			: leds(led_array+start),background(background_array+start) , ledCount(end-start) {}

		// Method to display segment properties
		void showSegmentInfo() {
			std::cout << "Number of LEDs: " << ledCount << std::endl;
		}

		// sets a uniform color
		void setColor(CRGB color){
			fill_solid(leds, ledCount, color);
			fill_solid(background, ledCount, color);
			FastLED.show();
		}
	};

	class LightStrip {
	private:
		CRGB* leds;
		CRGB* background;
		std::string chipType;
		int* ledMapping;
		int repeaterCount;

	public:
		AnimationManager animationManager;
		std::vector<StripSegment> segments; // Container for holding multiple strip segments
		const int ledCount;

		// Constructor to initialize the attributes and allocate memory for LEDs
		LightStrip(CRGB* led_array, CRGB* background_array, const int led_count, int* map, int repeater_count)
			: leds(led_array), background(background_array), ledCount(led_count), ledMapping(map), animationManager(leds, background, ledMapping, led_count), repeaterCount(repeater_count){
			std::cout << "LightStrip created at address: " << this << std::endl;
			// turning of repeaters
			for(int i=ledCount+repeaterCount-1; i>=ledCount; i--){
				leds[map[i]] = CRGB::Black;
				background[map[i]] = CRGB::Black;
			}
		}

		~LightStrip() {
			std::cout << "LightStrip destroyed at address: " << this << std::endl;
		}

		// Method to display LightStrip properties
		void showStripInfo() {
			std::cout << "Number of LEDs: " << ledCount << std::endl;
		}

		// Method to add a segment
		void addSegment(int start, int end) {
			segments.push_back(StripSegment(leds, background, start, end));  // Add a new segment to the vector
		}

		// Method to display segment information
		void showSegmentsInfo() {
			for (auto& segment : segments) {
				segment.showSegmentInfo(); // Show each segment's information
			}
		}

		// sets a uniform color
		void setColor(CRGB color){
			for(int i=0; i<ledCount; i++){
				leds[ledMapping[i]] = color;
				background[ledMapping[i]] = color;
			}
			FastLED.show();
		}

		void fillRangeWithColor(int low, int high, CRGB color) {
			low = max(0, low);
			high = min(ledCount - 1, high); // Höchstens NUM_LEDS - 1

			for (int i = low; i <= high; ++i) {
				leds[ledMapping[i]] = color;
			}
			FastLED.show();
		}

		void centeredLoadingBar(int count, CRGB color){
			fillRangeWithColor(ledCount/2 - count,ledCount/2 + count, color);
		}

		void spawnPulse(int pulseLength, double lamps_per_sec, int spawnPositon, int endPosition, CRGB pulseColor){
			animationManager.spawnPulse(pulseLength, lamps_per_sec, spawnPositon, endPosition, pulseColor);
		}

		void updateAnimation(){
			animationManager.update();
		}
	};

	// EspClient controller connects to a network and controls LightStrips
	class EspClient {
	private:
		const std::string ipAddress;
		const std::string networkPassword;
		const std::string networkSSID;
		WebServer server;

	public:
		std::vector<LightStrip> strips; // Container for holding multiple strip segments

		// Constructor to initialize the attributes
		EspClient(std::string ip, std::string password, std::string ssid)
			: ipAddress(ip), networkPassword(password), networkSSID(ssid), server(80) {}

		~EspClient() {
			std::cout << "EspClient destroyed at address: " << this << std::endl;
		}

		// Method to display network information
		void showNetworkInfo() {
			std::cout << "IP Address: " << ipAddress << std::endl;
			std::cout << "Network SSID: " << networkSSID << std::endl;
			std::cout << "Network Password: " << networkPassword << std::endl;
		}

		// Method to add a segment
		void addStrip(CRGB* led_array,CRGB* background_array, const int led_count, int* map, int repeater_count) {
			strips.push_back(LightStrip(led_array, background_array, led_count, map, repeater_count));  // Add a new segment to the vector
		}

		// Method to display segment information
		void showStripInfo() {
			for (auto& strip : strips) {
				strip.showStripInfo(); // Show each segment's information
			}
		}

		// turns all lamps off
		void handleOff(){
			for (auto& strip : strips) {
				strip.setColor(CRGB::Black);
			}
		}

		// sets a uniform RGB color
		void handleRgbColor(){
			int r = server.arg("r").toInt();
			int g = server.arg("g").toInt();
			int b = server.arg("b").toInt();
			Serial.printf("got new RGB request %i\t%i\t%i\n", r, g, b);

			CRGB color = CRGB(r,g,b);
			for (LightStrip& strip : strips) {
				strip.setColor(color);
			}
		}

		void startTestAnimation(){
			for (LightStrip& strip : strips) {
				std::cout << "starting test animation" << std::endl;
				strip.spawnPulse(5, 50, 0, strip.ledCount, CRGB::Red); //spawning default pulse
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
			int i = 0;
			while (WiFi.status() != WL_CONNECTED) {
				if (strips.size()>0){
					i++;
					strips[0].centeredLoadingBar(i, CRGB::Red);
				}
				std::cout << ".";
				std::cout.flush();
				delay(200);
			}
			std::cout << "\nWLAN connected!" << std::endl;
			strips[0].setColor(CRGB::Black);

			server.on("/turnOff", HTTP_GET, [this](){this->handleOff(); });
			server.on("/RgbColor", HTTP_GET, [this](){this->handleRgbColor(); });
			server.on("/sendPulse", HTTP_GET, [this](){this->startTestAnimation(); });
			server.begin();
			std::cout << "Web server started!\n" << std::endl;
		}

		void updateAnimations(){
			for(LightStrip& strip : strips){
				strip.updateAnimation();
			}
		}

		// handle server requests from any clients
		void handleClient(){
			server.handleClient();
		}
	};

#endif
