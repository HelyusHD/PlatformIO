#include <Arduino.h>
#include "esp_now.h"
#include "WiFi.h"

void setup() {
    Serial.begin(115200);
    Serial.println("---------------\nrunning\n---------------");

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }
    uint32_t version;
    esp_now_get_version(&version);
    Serial.println("version: " + String(version));
    Serial.println("ESP-NOW initialized");
}

void loop() {
}