#include <networkConfig.h>

#ifndef SERVER
#define SERVER
    /*
    is the access point (hosts the network)
    hosts the webserver
    will store config data for clients
    will log all clinets and their data
    */
    class myServer
    {
    private:
        const char* apPassword = ESP_NETWORK_PASSWORD;
        const char* ssid = ESP_NETWORK_SSID;
        WiFiServer server; // server from <WiFi.h>
        TaskHandle_t Task1; // required for dedicated core
        static void handleUiClient(void * parameter); // hosts the webserver (UI side)
        int data = 0;
    public:
        myServer(/* args */);
        ~myServer();
        int apSetup();
        void loadTestData();
        void printTestData();
    };
#endif