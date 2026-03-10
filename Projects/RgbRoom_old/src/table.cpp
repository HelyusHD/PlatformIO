#include <Arduino.h>
#include "myClient.h"
#include "networkConfig.h"
#include <FastLED.h>
#include "colorFunctions.h"

const int led_count = 432;

#define LED_PIN_FL 5
#define LED_PIN_R 4
#define NUM_LEDS_FL 324
#define NUM_LEDS_R 108

std::string ip = "192.168.4.";
EspClient espClient(ip+std::to_string(clients[0]), ESP_NETWORK_PASSWORD, ESP_NETWORK_SSID, "0");
CRGB leds[led_count];
CRGB background[led_count];
int ledMapping[led_count];

void setup() {
	Serial.begin(115200); // Start the serial communication at 115200 baud rate
	FastLED.addLeds<WS2815, LED_PIN_R, GRB>(leds, 0, NUM_LEDS_R); // Erster Strang
  FastLED.addLeds<WS2815, LED_PIN_FL, GRB>(leds, NUM_LEDS_R, NUM_LEDS_FL); // Zweiter Strang
	for(int i=0;i<led_count;i++){
		if(i<NUM_LEDS_R){
		ledMapping[i] = NUM_LEDS_R - i - 1;
		} else {
		ledMapping[i] = i;
		}
	}
	espClient.addStrip(leds, background, led_count, ledMapping, 0);
	//espClient.strips[0].animationManager.newPulseSource(10, 20.0, 0, 316, CRGB::Green, 1000);
	//espClient.strips[0].animationManager.newPulseSource(10, 20.0, 431, 116, CRGB::Blue, 1000);
	//espClient.strips[0].animationManager.newColorFunction(led_count,ColorFunctions::Rainbow(1000,50,255,20));

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
