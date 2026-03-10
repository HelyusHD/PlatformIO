#include <Arduino.h>
//#include "myServer-gpt.h"
#include "myServer.h"
#include "networkConfig.h"


// Create an ESP32 object (network information)
EspServer myESP32(ESP_NETWORK_PASSWORD,ESP_NETWORK_SSID,"esp",HOME_NETWORK_PASSWORD, HOME_NETWORK_SSID);
void setup() {
	Serial.begin(115200); // Start the serial communication at 115200 baud rate
	myESP32.networkingSetup();
}

void loop() {
	myESP32.handleClient();
}
