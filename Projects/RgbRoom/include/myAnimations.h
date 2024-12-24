#include <FastLED.h>
#include <cmath>
#include <iostream>

#ifndef MY_ANIMATIONS
#define MY_ANIMATIONS

class Pulse{
    private:
        CRGB* leds; //led array the animation runs on
        int ledCount;
        int lenght; //lenght of impulse
        float step; // lenght of step forwards a pulse runs in a tick
        float position; // position of pulse
        int* map; // mapping for leds
        CRGB* buffer;
        CRGB color;

    public:
        // constructor
        Pulse(CRGB* led_array, int led_count, int pulseLength = 1, float stepSize = 1, float spawnPositon = 0.0,CRGB pulseColor = CRGB::Black , int* mapping = {})
            : leds(led_array), ledCount(led_count), lenght(pulseLength), step(stepSize), position(spawnPositon), color(pulseColor), map(mapping){
                buffer = new CRGB[ledCount];
            }

        // updates animation and return 1 if animation is done and can be deleted
        int update(){
            if(position >= ledCount){return 1;}
            // changing color
            for(float f=position; f < position+lenght; f+=step){
                int i = static_cast<int>(std::floor(f));
                int i_mapped = map[i];
                if (i_mapped < ledCount){
                    buffer[i] = leds[i_mapped];
                    leds[i_mapped] = color;
                }
            }
            FastLED.show();
            // undoing changes to led_array
            for(float f=position; f < position+lenght; f+=step){
                int i = static_cast<int>(std::floor(f));
                int i_mapped = map[i];
                if (i_mapped < ledCount){
                    leds[i_mapped] = buffer[i];
                }
            }
            return 0;        
        }
        ~Pulse() {
            delete[] buffer; // Speicher freigeben
        }
};


#endif