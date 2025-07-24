#include <Arduino.h>

void setup()
{
pinMode(8, OUTPUT); // Pin 8 auf Ausgang schalten
// Pin 7 (Port D.7) internen Pull-Up Widerstand aktivieren
pinMode(7, INPUT_PULLUP);
}
void loop()
{
if(!digitalRead(7)) // ist der Pegel an Pin 7 low?
{
digitalWrite(8, HIGH); // LED an Pin 8 anschalten
}
else
{
digitalWrite(8, LOW); // LED an Pin 8 ausschalten
}
}