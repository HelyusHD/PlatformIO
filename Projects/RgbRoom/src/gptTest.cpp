#include <Arduino.h>
#include "myClient.h"
#include "networkConfig.h"
#include <FastLED.h>

// LED-Konfiguration
const int led_count = 432;

#define LED_PIN_FL 5
#define LED_PIN_R 4
#define NUM_LEDS_FL 324
#define NUM_LEDS_R 108

// Globale Objekte und Arrays
EspClient myESP32(CLIENT_01, ESP_NETWORK_PASSWORD, ESP_NETWORK_SSID);
CRGB leds[led_count];
CRGB background[led_count];
int ledMapping[led_count];

// Task-Handles
TaskHandle_t handleClientTask;
TaskHandle_t updateAnimationsTask;

// Task-Funktion: handleClient
void handleClientTaskFunction(void* parameter) {
    while (true) {
        myESP32.handleClient();
        vTaskDelay(10 / portTICK_PERIOD_MS); // Kleine Verzögerung, um CPU-Überlastung zu vermeiden
    }
}

// Task-Funktion: updateAnimations
void updateAnimationsTaskFunction(void* parameter) {
    while (true) {
        myESP32.updateAnimations();
    }
}

void setup() {
    Serial.begin(115200); // Starten der seriellen Kommunikation
    FastLED.addLeds<WS2815, LED_PIN_R, GRB>(leds, 0, NUM_LEDS_R); // Erster Strang
    FastLED.addLeds<WS2815, LED_PIN_FL, GRB>(leds, NUM_LEDS_R, NUM_LEDS_FL); // Zweiter Strang
    for (int i = 0; i < led_count; i++) {
        if (i < NUM_LEDS_R) {
            ledMapping[i] = NUM_LEDS_R - i - 1;
        } else {
            ledMapping[i] = i;
        }
    }
    myESP32.addStrip(leds, background, led_count, ledMapping);

    // Netzwerk und Streifeninformationen anzeigen
    myESP32.showNetworkInfo();
    myESP32.networkingSetup();
    myESP32.showStripInfo();

    // Tasks erstellen
    xTaskCreate(
        handleClientTaskFunction, // Task-Funktion
        "HandleClientTask",       // Name der Task (nur zu Debugging-Zwecken)
        4096,                     // Stack-Größe (in Bytes)
        NULL,                     // Parameter für die Task (optional)
        1,                        // Priorität der Task
        &handleClientTask         // Task-Handle
    );

    xTaskCreate(
        updateAnimationsTaskFunction, // Task-Funktion
        "UpdateAnimationsTask",       // Name der Task
        4096,                         // Stack-Größe
        NULL,                         // Parameter
        1,                            // Priorität
        &updateAnimationsTask         // Task-Handle
    );
}

void loop() {
    // Hauptschleife leer lassen oder andere Aufgaben hier ausführen
}
