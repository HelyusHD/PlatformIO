#include <Arduino.h>
#include "esp_now.h"
#include "WiFi.h"
#include "networkConfig.h"

#include <Logger.h>


int logSetup(){
    Serial.begin(115200);
    delay(1000);
    Logger::setLogLevel(Logger::VERBOSE);


    WiFi.mode(WIFI_STA);
    delay(100);
    String mac = WiFi.macAddress();
    Logger::notice("MAC: " + mac);

    if (esp_now_init() != ESP_OK) {
        Logger::fatal("ESP-NOW init failed");
        return 1;
    }
    uint32_t version;
    esp_now_get_version(&version);
    Logger::verbose("ESP-NOW version: " + String(version));
    return 0;
}

typedef struct {
    int counter;
} Message;

#ifdef DEVICE_ROLE_SENDER

    uint8_t peerAddress[] = MAC_WHITE_CABLE; // MAC of receiver ESP32

    void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
        Serial.print("Send status: ");
        Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
    }

    void setup() {
        if(logSetup() == 1) return;

        esp_now_register_send_cb(onSent); // what happens on send
        // creating peer for a reciving client
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, peerAddress, 6);
        peerInfo.channel = 0;  
        peerInfo.encrypt = false;
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Logger::fatal("Failed to add peer");
            return;
        }
        Logger::notice("ESP-NOW initialized");
    }

    Message msg;

    void loop() {
        esp_err_t result = esp_now_send(peerAddress, (uint8_t *) &msg, sizeof(msg));

        if (result == ESP_OK) {
            Logger::verbose("success sending");
        } else {
            Logger::verbose("failed sending");
        }

        msg.counter++;
        delay(10);
    }
#endif

#ifdef DEVICE_ROLE_RECEIVER
    void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len)
    {
        Message msg;

        if (len != sizeof(msg)) {
            Logger::error("Unexpected packet size: " + String(len));
            return;
        }

        memcpy(&msg, incomingData, sizeof(msg));

        Logger::verbose("Received from " + String(mac[0])+":" + String(mac[1])+":" + String(mac[2])+":" + String(mac[3])+":" + String(mac[4])+":" + String(mac[5]));
        Logger::verbose("Counter: " + String(msg.counter));
    }

    void setup(){
        if(logSetup() == 1) return;
        esp_now_register_recv_cb(onReceive);
    }
    void loop(){}
#endif