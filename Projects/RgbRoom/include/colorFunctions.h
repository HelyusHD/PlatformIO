#ifndef COLOR_FUNCTIONS_H
#define COLOR_FUNCTIONS_H

#include <FastLED.h>
#include <functional>

// Einige nützliche Funktionen für CRGB(int, unsigned long)
namespace ColorFunctions {

    // Regenbogen-Gradient
    class Rainbow {
    private:
        unsigned long periodTime;
    public:
        Rainbow(unsigned long period) : periodTime(period) {}
        CRGB operator()(int index, unsigned long time) const {
            uint8_t hue = (index * 10 + (time / periodTime) % 255);
            return CHSV(hue, 255, 255);
        }
    };

    // Farbverlauf von Blau zu Rot
    class BlueToRed {
    private:
        float frequency;
    public:
        BlueToRed(float freq) : frequency(freq) {}
        CRGB operator()(int index, unsigned long time) const {
            uint8_t progress = (sin((index + time / frequency) * 0.1) + 1) * 127.5;
            return CRGB(progress, 0, 255 - progress);
        }
    };

    // Blinken in einer zufälligen Farbe
    class RandomBlink {
    private:
        unsigned long blinkTime;
    public:
        RandomBlink(unsigned long time) : blinkTime(time) {}
        CRGB operator()(int index, unsigned long time) const {
            if ((time / blinkTime) % 2 == 0) {
                uint8_t hue = (index * 37) % 255;
                return CHSV(hue, 255, 255);
            } else {
                return CRGB::Black;
            }
        }
    };

    // Welle (Sinus-basiert, bewegt sich durch die LEDs)
    class Wave {
    private:
        float speed;
    public:
        Wave(float waveSpeed) : speed(waveSpeed) {}
        CRGB operator()(int index, unsigned long time) const {
            uint8_t brightness = (sin((index * 0.3) + (time * speed)) + 1) * 127.5;
            return CRGB(0, 0, brightness);
        }
    };

    // Farbverlauf über die LEDs mit Zeitverzögerung
    class Gradient {
    private:
        unsigned long gradientSpeed;
    public:
        Gradient(unsigned long speed) : gradientSpeed(speed) {}
        CRGB operator()(int index, unsigned long time) const {
            uint8_t hue = (index * 5 + (time / gradientSpeed) % 255);
            return CHSV(hue, 255, 255);
        }
    };

    // Feuerähnlicher Effekt
    class Fire {
    private:
        unsigned long cooling;
    public:
        Fire(unsigned long cool) : cooling(cool) {}
        CRGB operator()(int index, unsigned long time) const {
            uint8_t heat = qsub8(random8((index * 37 + time / cooling) % 255), random8(50));
            return HeatColor(heat);
        }
    };

    // Pulsierende Farbe
    class Pulse {
    private:
        float pulseSpeed;
    public:
        Pulse(float speed) : pulseSpeed(speed) {}
        CRGB operator()(int index, unsigned long time) const {
            uint8_t brightness = (sin(time / pulseSpeed) + 1) * 127.5;
            uint8_t hue = (index * 5) % 255;
            return CHSV(hue, 255, brightness);
        }
    };

}

#endif // COLOR_FUNCTIONS_H