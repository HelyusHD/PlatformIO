#include <Arduino.h>
#include "myClient.h"
#include "networkConfig.h"
#include <FastLED.h>

const int led_count = 83; 
const int repeater_count = 1;

// Create an EspClient object
std::string ip = "192.168.4.";
EspClient espClient(ip+std::to_string(clients[1]), ESP_NETWORK_PASSWORD, ESP_NETWORK_SSID, "1");
CRGB leds[led_count+repeater_count];
CRGB background[led_count+repeater_count];
int ledMapping[led_count+repeater_count];

void setup() {
	Serial.begin(115200); // Start the serial communication at 115200 baud rate
	FastLED.addLeds<WS2811, 5>(leds, led_count+repeater_count);

	for(int i=0;i<led_count;i++){
		if(i<56){
			ledMapping[i] = i;
		}else{ // excluding repeater from map
			ledMapping[i] = i+1;
		}
	}
	ledMapping[led_count] = 56; // mapping repeater to end of array

	espClient.addStrip(leds, background, led_count, ledMapping, repeater_count);
	espClient.strips[0].addSegment(0, 56);
	espClient.strips[0].addSegment(57, 82);
	//espClient.strips[0].segments[0].setColor(CRGB::Red);

	
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
