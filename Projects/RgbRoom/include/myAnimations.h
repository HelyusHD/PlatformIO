#include <FastLED.h>
#include <iostream>
#include <vector>

#ifndef MY_ANIMATIONS
#define MY_ANIMATIONS

class Pulse {
private:
    int ledCount;
    int length; // length of impulse
    double speed; // speed in lamps/s
    int shift; // position of pulse
    std::vector<CRGB> buffer;
    CRGB color;
    unsigned long spawnTick;

public:
    bool done;

    // Constructor
    Pulse(int pulseLength, double lamps_per_sec, int spawnPosition, CRGB pulseColor)
        : length(pulseLength), speed(lamps_per_sec), shift(spawnPosition), color(pulseColor), buffer(length), done(false), spawnTick(millis()) {
        std::cout << "### creating new Pulse ### at address: " << this << std::endl;
    }

    //Pulse(Pulse& other) = delete;
    //Pulse &operator=(Pulse& other) = delete;

    // Destructor
    ~Pulse() {
        std::cout << "Pulse destroyed at address: " << this << std::endl;
    }

    // Update animation and return true if animation is done and can be deleted
    bool update(CRGB* leds, int* map, int ledCount) {

        // Changing color
        double dt = (millis() - spawnTick);
        int pos = std::floor(speed * dt / 1000) + shift;
        std::cout << "pos: " << pos << std::endl;
        if (pos>ledCount){return true;}
        for(int i = pos; i <= pos + length; i++){
            if (i < ledCount) {
                leds[map[i]] = color;
            }else{break;}
        }
        return false;
    }
};

class PulseManager {
private:
    CRGB* leds; // LED array to perform animation on
    CRGB* background;
    int* map; // Defines custom order of LEDs
    std::vector<Pulse> pulses; // Container for holding multiple pulses
    int ledCount;

public:
    PulseManager(CRGB* ledArray, CRGB* background_array, int* ledMapping, int led_count)
        : leds(ledArray), background(background_array), map(ledMapping), ledCount(led_count) {
    }

    ~PulseManager() {
        std::cout << "PulseManager destroyed at address: " << this << std::endl;
    }

    void spawnPulse(int pulseLength, double lamps_per_sec, float spawnPosition, CRGB pulseColor) {
        pulses.emplace_back(pulseLength, lamps_per_sec, spawnPosition, pulseColor);
    }

    /*void update() {
        for (auto it = pulses.begin(); it != pulses.end(); ) {
            if (it->update(leds, map)) {
                std::cout << "Removing completed Pulse." << std::endl;
                it = pulses.erase(it);
            } else {
                ++it;
            }
        }
    }*/

    void update() {
        for(int i = 0; i<ledCount; i++){
            background[i] = leds[i];
        }
        for(int i = 0;i<pulses.size();){
            bool var = pulses.at(i).update(leds, map, ledCount);
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
