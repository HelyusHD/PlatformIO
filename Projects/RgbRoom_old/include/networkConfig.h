#ifndef NETWORK_CONFIG
#define NETWORK_CONFIG
    #define HOME_NETWORK_PASSWORD "85505796012028034369"
    #define HOME_NETWORK_SSID "BuechseDerPandora"

    #define ESP_NETWORK_PASSWORD  "21345678"
    #define ESP_NETWORK_SSID "ESP32_Network"

    //#define CLIENT_01 "192.168.4.2"
    //#define CLIENT_02 "192.168.4.3"
    int clients[3] = {2,3,4};
    int tasmota[4] = {10,11,12,13}; // same list index = tasmota powers that client
#endif
// Backlog IPAddress1 192.168.4.10; IPAddress2 192.168.4.1; IPAddress3 255.255.255.0; IPAddress4 192.168.4.1
