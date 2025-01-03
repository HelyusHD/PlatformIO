#include <Arduino.h>
#include "myClient.h"
#include "networkConfig.h"
#include <FastLED.h>

const int led_count = 151; 
const int repeater_count = 0;

// Create an EspClient object
std::string ip = "192.168.4.";
EspClient espClient(ip+std::to_string(clients[2]), ESP_NETWORK_PASSWORD, ESP_NETWORK_SSID, "2");
CRGB leds[led_count+repeater_count];
CRGB background[led_count+repeater_count];
int ledMapping[led_count+repeater_count];

void setup() {
	Serial.begin(115200); // Start the serial communication at 115200 baud rate
	FastLED.addLeds<WS2815, 5>(leds, led_count+repeater_count);

	for(int i=0;i<led_count;i++){
		ledMapping[i] = i;
	}

	espClient.addStrip(leds, background, led_count, ledMapping, repeater_count);

	// Display network information from ESP32
	espClient.showNetworkInfo();
	espClient.networkingSetup();

	// Show strip info
	espClient.showStripInfo();
}

void loop() {
	espClient.handleClient();
	espClient.updateAnimations();
}
