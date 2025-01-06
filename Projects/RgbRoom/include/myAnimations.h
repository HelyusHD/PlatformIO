#include <FastLED.h>
#include <iostream>
#include <vector>
#include <functional>

#ifndef MY_ANIMATIONS
#define MY_ANIMATIONS

// handles the behavior and lifecycle of a single pulse
class Pulse {
private:
	int ledCount;
	int length; // length of impulse
	double speed; // speed in lamps/s
	int spawnPosition; // position of pulse
	int endPosition;
	CRGB color;
	unsigned long spawnTick = millis();
	unsigned long delay; // delay between planed and actual spawn time of the pulse

public:
	bool done = false;

	// Constructor
	Pulse(int pulseLength, double lamps_per_sec, int spawn_position, int end_position, CRGB pulseColor, unsigned long delay_in_ms = 0)
		: length(pulseLength), speed(lamps_per_sec), spawnPosition(spawn_position), endPosition(end_position), color(pulseColor), delay(delay_in_ms){
		//std::cout << "### creating new Pulse ### at address: " << this << std::endl;
		if(endPosition<spawnPosition){speed = -speed;} //making sure that the impulse travels from spawn to end
	}

	// Destructor
	~Pulse() {
		//std::cout << "Pulse destroyed at address: " << this << std::endl;
	}

	// Update animation and return true if animation is done and can be deleted
	bool update(CRGB* leds, int* map) {
		// Changing color
		double dt = (millis() - spawnTick + delay);
		int pos = std::round(speed * dt / 1000) + spawnPosition;
		if (speed > 0){
			if (pos - length > endPosition || pos < 0){return true;}
			for(int i = pos; i > pos - length; i--){
				if (i >= spawnPosition && i < endPosition) {
					leds[map[i]] = color;
				}
			}
		}else{
			if (pos < endPosition){return true;}
			for(int i = pos; i > pos - length; i--){
				if (i >= endPosition && i < spawnPosition) {
					leds[map[i]] = color;
				}
			}
		}
		return false;
	}
};

// manages the periodic creation of pulses
class PulseSource{
private:
	const int periodTime; // delay in ms between spawns of pulses
	unsigned long spawnTick = millis();
	// Pulse 
	int ledCount;
	int length; // length of impulse
	double speed; // speed in lamps/s
	int spawnPosition; // position of pulse
	int endPosition;
	CRGB color;
public:
	PulseSource(int pulse_length, double lamps_per_sec, int spawn_position, int end_position, CRGB pulseColor, const int period_time)
	: length(pulse_length), speed(lamps_per_sec), spawnPosition(spawn_position), endPosition(end_position), color(pulseColor), periodTime(period_time){
		std::cout << "created new PulseSource" << std::endl;
	}

	Pulse* update() {
		if (millis() > spawnTick + periodTime) {
			spawnTick += periodTime;
			auto* newPulse = new Pulse(length, speed, spawnPosition, endPosition, color, millis() - spawnTick);
			//std::cout << "created new Pulse t: " << millis() << "   delay: " << millis() - spawnTick << std::endl;
			return newPulse;  // Return dynamically allocated Pulse
		}
		return nullptr;  // No new pulse
	}

};

// controlls colors based on a rgb(s,t) function
class ColorFunktion{
private:
	unsigned long spawnTick = millis();
	int ledCount;
	std::function<CRGB(int, unsigned long)> func;
public:
	// constructor
	ColorFunktion(int led_count, std::function<CRGB(int, unsigned long)> function)
	: func(function),ledCount(led_count){};

	void update(CRGB* leds, int* map){
		for(int i=0; i<ledCount; i++){
			leds[map[i]] = func(i, millis()-spawnTick);
		}
	}
};

// orchestrates and updates all animations and sources
class AnimationManager {
private:
	CRGB* leds; // LED array to perform animation on
	CRGB* background;
	int ledCount;
	int* map; // Defines custom order of LEDs
	std::vector<Pulse> pulses; // Container for holding pulses
	std::vector<PulseSource> pulseSources; // Container for holding pulseSources
	std::vector<ColorFunktion> colorFunctions; // Container for holding colorFunctions

public:
	AnimationManager(CRGB* ledArray, CRGB* background_array, int* ledMapping, int led_count)
		: leds(ledArray), background(background_array), ledCount(led_count), map(ledMapping) {
		std::cout << "AnimationManager created at address: " << this << std::endl;
	}

	~AnimationManager() {
		std::cout << "AnimationManager destroyed at address: " << this << std::endl;
	}

	void spawnPulse(int pulseLength, double lamps_per_sec, int spawn_position, int endPosition, CRGB pulseColor) {
		pulses.emplace_back(pulseLength, lamps_per_sec, spawn_position, endPosition, pulseColor);
	}

	// adds a pulse by creating a copy from a template
	void addPulse(Pulse pulse){
		pulses.emplace_back(pulse);
	}

	void newPulseSource(int pulse_length, double lamps_per_sec, int spawn_position, int end_position, CRGB pulseColor, const int period_time){
		pulseSources.emplace_back(pulse_length, lamps_per_sec, spawn_position, end_position, pulseColor, period_time);
	}

	void newColorFunction(int led_count, std::function<CRGB(int, unsigned long)> function){
		colorFunctions.emplace_back(led_count, function);
	}

	void update() {
		// safing state of LED array
		for(int i = 0; i<ledCount; i++){
			background[i] = leds[i];
		}

		for (ColorFunktion& colorFunction : colorFunctions){
			colorFunction.update(leds, map);
		}

		// pulseSources
		for (PulseSource& pulseSource : pulseSources) {
			Pulse* pulse = pulseSource.update();
			if (pulse) {
				pulses.emplace_back(*pulse);  // Add by copying the pulse
				delete pulse;                // Clean up the dynamically allocated pulse
			}
		}

		// pulses
		for(int i = 0;i<pulses.size();){
			bool var = pulses.at(i).update(leds, map);
			if(var){
				pulses.erase(pulses.begin()+i);
			}else{
				i++;
			}
		}

		FastLED.show();

		// restoring state LED array
		for(int i = 0; i<ledCount; i++){
			leds[i] = background[i];
		}
	}
};

#endif
