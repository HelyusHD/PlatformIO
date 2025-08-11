#include <WiFi.h> //connecting to network
#include <ESPAsyncWebServer.h> //webserver that can load directly from SD without loading into RAM
#include <ESPmDNS.h> //simple DNS
#include <mySdManager.h>
#include <networkConfig.h> // passwords and stuff
#include <myLog.h>
#include <AsyncTCP.h> // for live broadcasting values

#ifndef MY_SERVER
#define MY_SERVER

    struct MyData { // test data
        int number;
        char text[100];
    };

    // responsible for the webserver
    class MyWebserver{
        private:
            AsyncWebServer server; // webserver
            AsyncWebSocket ws; // websocket
            fs::FS &fs; // file system

            uint32_t last = 0; // counter
            void setupRoutesFromSD();   // defines what file should be hosted
        public:
            MyWebserver(fs::FS& _fs)
                : server(80), ws("/ws"), fs(_fs){}

            ~MyWebserver(){}

            void begin();   // starts the webserver

            volatile int someValue = 0; // test value for websocket
            void broadcastTestData(const MyData& data); // send to all connected clients
            void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
            void tick();
    };

    // Wifi connection
    class MyServer{
        private:
            MySdManager& sdM;
        public:
            MyWebserver webserver;

            MyServer(MySdManager& _sdM)
            : sdM(_sdM), webserver(_sdM.getFS()) {}    // creates a MyServer and initiates MySdManager and MyWebserver

            ~MyServer(){};

            void connectToNetwork(const char* dnsName = "test_server"); // connecting to an existing network using a DNS name
            void setupRoutes(){
                int a = 1;
            }
    };

    void MyWebserver::broadcastTestData(const MyData& data){
        char buffer[150];
        snprintf(buffer, sizeof(buffer), "{\"number\":%d,\"text\":\"%s\"}", data.number, data.text);
        ws.textAll(buffer);
    }

    void MyWebserver::tick(){
        ws.cleanupClients();
        // Example: update every second
        if (millis() - last > 1000) {
            last = millis();
            MyData tmp;
            tmp.number = someValue++;
            strcpy(tmp.text, "Hello World");
            broadcastTestData(tmp);
        }
    }

    void MyWebserver::begin(){
        setupRoutesFromSD();

        ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
            this->onWsEvent(server, client, type, arg, data, len);
        });
        server.addHandler(&ws);

        server.begin();
        LOG(LOG_INFO,"launched webserver");
    }

    void MyWebserver::setupRoutesFromSD(){
        // Serve everything; default = index.html
        server.serveStatic("/", fs, "/") // will look for files in "/"
                .setDefaultFile("index.html")
                .setCacheControl("max-age=3600");   // tweak as you like

        // routs /page to /
        server.on("/page", HTTP_GET, [](AsyncWebServerRequest* req){
            req->redirect("/");
        });

        // error 404 fallback
        server.onNotFound([this](AsyncWebServerRequest* req){
            String path = req->url();
            if (path.endsWith("/")) path += "index.html";
            if (fs.exists(path))     req->send(fs, path, String());
            else                     req->send(404, "text/plain", "Not found");
        });
        }

    void MyWebserver::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
        if (type == WS_EVT_CONNECT) {
            Serial.printf("Client %u connected\n", client->id());
            client->text(String(someValue)); // send current value immediately
        }
        else if (type == WS_EVT_DISCONNECT) {
            Serial.printf("Client %u disconnected\n", client->id());
        }
        else if (type == WS_EVT_DATA) {
            String msg((char*)data, len);
            Serial.printf("Got message: %s\n", msg.c_str());
        }
    }
    
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