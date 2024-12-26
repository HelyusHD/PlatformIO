#include <Arduino.h>
#include "myClient.h"
#include "networkConfig.h"
#include <FastLED.h>

const int led_count = 83;

// Create an ESP32 object (network information)
EspClient myESP32(CLIENT_02, ESP_NETWORK_PASSWORD, ESP_NETWORK_SSID);
CRGB leds[led_count];
CRGB background[led_count];
int ledMapping[led_count];

void setup() {
    Serial.begin(115200); // Start the serial communication at 115200 baud rate
    FastLED.addLeds<WS2811, 5>(leds, led_count);
    for(int i=0;i<led_count;i++){
        ledMapping[i] = i;
    }
    myESP32.addStrip(leds, background, led_count, ledMapping);
    myESP32.strips[0].addSegment(0, 56);
    myESP32.strips[0].addSegment(57, 82);
    //myESP32.strips[0].segments[0].setColor(CRGB::Red);

    
    // Display network information from ESP32
    myESP32.showNetworkInfo();
    myESP32.networkingSetup();

    // Show strip info
    myESP32.showStripInfo();
}

void loop() {
    myESP32.handleClient();
    myESP32.updateAnimations();
}
