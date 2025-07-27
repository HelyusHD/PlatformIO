#include <networkConfig.h>
#include <Arduino.h>
#include <WiFi.h>

#ifndef SERVER
#define SERVER
    class myServer
    {
    private:
        const char* apPassword = ESP_NETWORK_PASSWORD;
        const char* ssid = ESP_NETWORK_SSID;
        WiFiServer server;
        TaskHandle_t Task1;
        static void handleClient(void * parameter);
    public:
        myServer(/* args */);
        ~myServer();
    };
#endif