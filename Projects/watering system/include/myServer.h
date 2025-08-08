#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <mySdManager.h>
#include <networkConfig.h>
#include <myLog.h>

#ifndef MY_SERVER
#define MY_SERVER

    

    class MyServer{
        private:
            MySdManager& sdM;
            AsyncWebServer server;
        public:
            MyServer(MySdManager& _sdM);
            ~MyServer();
            void connectToNetwork(const char* dnsName = "test_server"); // connecting to an existing network using a DNS name
            void setupRoutes(){
                int a = 1;
            }

    };

    MyServer::MyServer(MySdManager& _sdM)
        : sdM(_sdM), server(80){}
    
    MyServer::~MyServer(){}

    void MyServer::connectToNetwork(const char* dnsName){
        // connecting to home network
        LOG(LOG_DEBUG,String("connecting to network: ") + HOME_NETWORK_SSID);
        WiFi.begin(HOME_NETWORK_SSID, HOME_NETWORK_PASSWORD);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            LOG(LOG_DEBUG,"waiting for connection");
        }
        LOG(LOG_INFO,String("successfully connected to network: ") + HOME_NETWORK_SSID);

        // using mDNS to allow a dynamic IP
        if (MDNS.begin(dnsName)) {  // no ".local" here
            LOG(LOG_DEBUG, String("mDNS name: ") + dnsName);
        }
    }

    
#endif