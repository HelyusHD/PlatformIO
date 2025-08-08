#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <mySdManager.h>
#include <networkConfig.h>
#include <myLog.h>

#ifndef MY_SERVER
#define MY_SERVER

    class MyWebserver{
        private:
            AsyncWebServer server;
            fs::FS &fs;

            void setupRoutesFromSD();   // defines what file should be hosted
        public:
            MyWebserver(fs::FS& _fs)
                : server(80), fs(_fs){}

            ~MyWebserver(){}

            void begin(){   // starts the webserver
                setupRoutesFromSD();
                server.begin();
                LOG(LOG_INFO,"launched webserver");
            };
    };

    // handles Wifi connection
    class MyServer{
        private:
            MySdManager& sdM;
        public:
            MyWebserver webserver;
            MyServer(MySdManager& _sdM);    // creates a MyServer and initiates MySdManager and MyWebserver

            ~MyServer();

            void connectToNetwork(const char* dnsName = "test_server"); // connecting to an existing network using a DNS name
            void setupRoutes(){
                int a = 1;
            }
    };



    void MyWebserver::setupRoutesFromSD(){
            // 1) Serve everything; default = index.html
            server.serveStatic("/", fs, "/") // will look for files in "/"
                    .setDefaultFile("index.html")
                    .setCacheControl("max-age=3600");   // tweak as you like

            // 2) Optional: /page -> /
            server.on("/page", HTTP_GET, [](AsyncWebServerRequest* req){
                req->redirect("/");
            });

            // 3) Helpful 404 (also covers weird paths)
            server.onNotFound([this](AsyncWebServerRequest* req){
                String path = req->url();
                if (path.endsWith("/")) path += "index.html";
                if (fs.exists(path))     req->send(fs, path, String());
                else                     req->send(404, "text/plain", "Not found");
            });
            }

    MyServer::MyServer(MySdManager& _sdM)
        : sdM(_sdM), webserver(_sdM.getFS()) {}
    
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