#include <Arduino.h>
#include "myClient.h"
#include "networkConfig.h"
#include <FastLED.h>

const int led_count = 432;

#define LED_PIN_FL 5
#define LED_PIN_R 4
#define NUM_LEDS_FL 324
#define NUM_LEDS_R 108

// Create an ESP32 object (network information)
EspClient espClient(CLIENT_01, ESP_NETWORK_PASSWORD, ESP_NETWORK_SSID);
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
    //espClient.strips[0].animationManager.newPulseSource(1, 100.0, 0, 215, CRGB::Green, 200);
    //espClient.strips[0].animationManager.newPulseSource(1, 100.0, 431, 215, CRGB::Blue, 200);

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
