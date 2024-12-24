#include <Arduino.h>
#include "myClient.h"
#include "networkConfig.h"
#include <FastLED.h>

#define LED_COUN_01 83

// Create an ESP32 object (network information)
EspClient myESP32(CLIENT_02, ESP_NETWORK_PASSWORD, ESP_NETWORK_SSID);
CRGB leds[LED_COUN_01];
int ledMapping[LED_COUN_01];

void setup() {
    Serial.begin(115200); // Start the serial communication at 115200 baud rate
    FastLED.addLeds<WS2811, 5>(leds, LED_COUN_01);
    for(int i=0;i<LED_COUN_01;i++){
        ledMapping[i] = i;
    }
    myESP32.addStrip(leds, 83, ledMapping);
    //myESP32.strips[0].addSegment(0, 56);
    myESP32.strips[0].addSegment(57, 82);
    myESP32.strips[0].segments[0].setColor(CRGB::Red);

    
    // Display network information from ESP32
    myESP32.showNetworkInfo();
    myESP32.networkingSetup();

    // Show strip info
    myESP32.showStripInfo();
}

void loop() {
    myESP32.handleClient();
}
