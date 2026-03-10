#include <Arduino.h>
#include "esp_now.h"
#include "WiFi.h"
/*
#include <plog\Log.h> // the log lib
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ArduinoAppender.h>
*/

#include "networkConfig.h"


//plog::ArduinoAppender<plog::TxtFormatter> arduinoAppender(Serial); // makes logging over serial possible
int logSetup(){
    Serial.begin(115200); // opens serial connection
    //plog::init(plog::debug, &arduinoAppender); // starts the logger over serial

    PLOGD << "--- setup() running ---"; // log on DEBUG level

    WiFi.mode(WIFI_STA);
    delay(100);
    String mac = WiFi.macAddress();
    PLOGI << "macAddress: " << mac.c_str();

    if (esp_now_init() != ESP_OK) {
        PLOGE << "ESP-NOW init failed"; // log on ERROR level
        return 1;
    }
    uint32_t version;
    esp_now_get_version(&version);
    PLOGD << "ESP-NOW version: " << String(version).c_str(); // log on DEBUG level
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
        PLOGE << "Failed to add peer";
            return;
        }

        PLOGI << "ESP-NOW initialized";

        PLOGD << "--- setup() done ---"; // log on DEBUG level
    }

    Message msg;

    void loop() {
        esp_err_t result = esp_now_send(peerAddress, (uint8_t *) &msg, sizeof(msg));

        if (result == ESP_OK) {
            PLOGI << "Message queued";
        } else {
            PLOGI << "Send error";
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
            PLOGE << "Unexpected packet size: " << len;
            return;
        }

        memcpy(&msg, incomingData, sizeof(msg));

        Serial.printf("Received from %02X:%02X:%02X:%02X:%02X:%02X\n",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        PLOGI << "Counter: " << msg.counter;
    }

    void setup(){
        if(logSetup() == 1) return;
        esp_now_register_recv_cb(onReceive);
    }
    void loop(){}
#endif