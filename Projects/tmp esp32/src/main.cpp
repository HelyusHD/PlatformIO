#include <FastLED.h>
#include <math.h>

#define LED_PIN 5      // Pin für LED-Strip
#define NUM_LEDS 324   // Anzahl der LEDs 144 * 4
#define DELAY_TIME 0  // Verzögerung zwischen LED-Bewegungen (in Millisekunden)
CRGB leds[NUM_LEDS];

bool on = true;        // LED-Strip-Zustand
int offset = 0;        // Offset in Position der LEDs für Farbverschiebung

float T = 10000.0;         // Periodendauer in Millisekunden
int helligkeit_max = 255 * 1; // Maximale Helligkeit
int helligkeit_min = 255 * 1; // Minimale Helligkeit

// Funktion, um Regenbogenfarben mit HSV direkt zu erzeugen
void rainbow_color(int index, CHSV &color, int brightness) {
    // Kontinuierliche Berechnung des Farbtons basierend auf Offset
    float hue_ratio = (index + offset) % NUM_LEDS; // Position mit Offset
    hue_ratio = hue_ratio / NUM_LEDS;             // Verhältnis für Farbskalierung
    uint8_t hue = (uint8_t)(hue_ratio * 255.0);   // Hue von 0 bis 255
    color = CHSV(hue, 255, brightness);          // Farbe mit angegebener Helligkeit
}

// Berechnet die sinusförmige Helligkeit
int calculate_brightness(float current_time, float T, int min_brightness, int max_brightness) {
    float omega = 2.0 * M_PI / T; // Kreisfrequenz
    float normalized_brightness = (sin(omega * current_time) + 1) / 2.0; // Normiert auf [0, 1]
    return min_brightness + (int)(normalized_brightness * (max_brightness - min_brightness));
}

void setup() {
    FastLED.addLeds<WS2815, LED_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
    // Aktuelle Zeit in Millisekunden
    float current_time = millis() % (int)T; // Zeit innerhalb der aktuellen Periode

    // Berechnung der aktuellen Helligkeit
    int brightness = calculate_brightness(current_time, T, helligkeit_min, helligkeit_max);

    // LED-Animation, wenn aktiv
    if (on) {
        for (int i = 0; i < NUM_LEDS; i++) {
            CHSV color;
            rainbow_color(i, color, brightness);
            leds[i] = color;
        }

        offset += 1; // Offset für kontinuierliche Bewegung
        if (offset >= NUM_LEDS) offset = 0;

        FastLED.show(); // LEDs aktualisieren
        //delay(DELAY_TIME);
    }
}
