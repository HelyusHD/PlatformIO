#include <Arduino.h>
#include "esp_now.h"
#include "WiFi.h"
// comment
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void onDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
    Serial.print("Received ");
    Serial.print(len);
    Serial.println(" bytes");
}

void setup() {
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        return;
    }

    Serial.println("ESP-NOW initialized");

    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);
}

void loop() {
}