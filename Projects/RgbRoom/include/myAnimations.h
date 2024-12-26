#include <FastLED.h>
#include <iostream>
#include <vector>

#ifndef MY_ANIMATIONS
#define MY_ANIMATIONS

// animates a pulse
class Pulse {
private:
    int ledCount;
    int length; // length of impulse
    double speed; // speed in lamps/s
    int shift; // position of pulse
    int endPosition;
    CRGB color;
    unsigned long spawnTick;

public:
    bool done;

    // Constructor
    Pulse(int pulseLength, double lamps_per_sec, int spawnPosition, int end_position, CRGB pulseColor)
        : length(pulseLength), speed(lamps_per_sec), shift(spawnPosition), endPosition(end_position), color(pulseColor), done(false), spawnTick(millis()) {
        std::cout << "### creating new Pulse ### at address: " << this << std::endl;
    }

    // Destructor
    ~Pulse() {
        std::cout << "Pulse destroyed at address: " << this << std::endl;
    }

    // Update animation and return true if animation is done and can be deleted
    bool update(CRGB* leds, int* map) {

        // Changing color
        double dt = (millis() - spawnTick);
        int pos = std::floor(speed * dt / 1000) + shift;
        if (pos - length > endPosition){return true;}
        for(int i = pos; i >= pos - length; i--){
            if (i >= 0 && i < endPosition) {
                leds[map[i]] = color;
            }
        }
        return false;
    }
};

// collects and updates animations
class AnimationManager {
private:
    CRGB* leds; // LED array to perform animation on
    CRGB* background;
    int* map; // Defines custom order of LEDs
    std::vector<Pulse> pulses; // Container for holding pulses
    int ledCount;

public:
    AnimationManager(CRGB* ledArray, CRGB* background_array, int* ledMapping, int led_count)
        : leds(ledArray), background(background_array), map(ledMapping), ledCount(led_count) {
    }

    ~AnimationManager() {
        std::cout << "AnimationManager destroyed at address: " << this << std::endl;
    }

    void spawnPulse(int pulseLength, double lamps_per_sec, int spawnPosition, int endPosition, CRGB pulseColor) {
        pulses.emplace_back(pulseLength, lamps_per_sec, spawnPosition, endPosition, pulseColor);
    }

    void update() {
        for(int i = 0; i<ledCount; i++){
            background[i] = leds[i];
        }
        for(int i = 0;i<pulses.size();){
            bool var = pulses.at(i).update(leds, map);
            if(var){
                pulses.erase(pulses.begin()+i);
            }else{
                i++;
            }
        }
        FastLED.show();
        // Undoing changes to LED array
        for(int i = 0; i<ledCount; i++){
            leds[i] = background[i];
        }
    }
};

#endif
