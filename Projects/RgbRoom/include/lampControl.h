#include <FastLED.h>
#include <vector>

#ifndef LAMP_CONTROL
#define LAMP_CONTROL

CRGB getComplementaryColor(const CRGB& color) {
    return CRGB(255 - color.r, 255 - color.g, 255 - color.b);
}

// Struktur zur Beschreibung eines Impulses
struct Pulse {
    int position;           // Aktuelle Position des Impulses
    int tickStep;           // Anzahl der LEDs, die der Impuls pro Tick bewegt
    int length;             // Länge des Impulses in LEDs
    CRGB pulseColor;        // Farbe des Impulses
    bool complementary;
};

// Klasse zur Verwaltung der Impulse
class PulseManager {
private:
    std::vector<Pulse> pulses;  // Liste der aktiven Impulse
    CRGB *leds;                 // LED-Array
    int numLeds;                // Anzahl der LEDs
    CRGB* buffer;

public:
    PulseManager(CRGB *ledArray, int totalLeds) : leds(ledArray), numLeds(totalLeds) {
        buffer = new CRGB[numLeds];
    }

    // Impuls spawnen
    void spawnPulse(CRGB pulseColor, int tickStep, int spawnPosition, int length, bool complementary) {
        if (spawnPosition >= 0 && spawnPosition < numLeds) {
            pulses.push_back({spawnPosition, tickStep, length, pulseColor, complementary});
        }
    }

    // Alle Impulse aktualisieren und LEDs neu rendern
    void updatePulses(int ledMapping[]) {

        // Impulse verarbeiten
        for (auto it = pulses.begin(); it != pulses.end(); ) {
            Pulse &pulse = *it;

            // Impuls auf LEDs zeichnen
            for (int i = 0; i < pulse.length; i++) {
                int ledIndex = ledMapping[pulse.position + i];
                if (ledIndex >= 0 && ledIndex < numLeds) {
                    if(pulse.complementary){
                        buffer[ledIndex] = leds[ledIndex];
                        leds[ledIndex] = getComplementaryColor(leds[ledMapping[i]]);
                    } else {
                        buffer[ledIndex] = leds[ledIndex];
                        leds[ledIndex] = pulse.pulseColor;
                    }
                }
            }

            // Position des Impulses aktualisieren
            int prefPos = pulse.position;
            pulse.position += pulse.tickStep;

            // Impuls entfernen, wenn er aus dem sichtbaren Bereich heraus ist
            if (pulse.position >= numLeds || pulse.position + pulse.length < 0) {
                it = pulses.erase(it);
            } else {
                ++it;
            }

            // undo changes
            FastLED.show();
            for (int i = 0; i < pulse.length; i++) {
                int ledIndex = ledMapping[prefPos + i];
                if (ledIndex >= 0 && ledIndex < numLeds) {
                    leds[ledIndex] = buffer[ledIndex];
                }
            }
        }
    }
    ~PulseManager() {
        delete[] buffer; // Speicher freigeben
    }
};

#endif