#include <Arduino.h>
#include <Wire.h>

// Senden und Empfangen 
#define SENDEN 9
#define ECHO 8

void setup()
{
  pinMode(SENDEN, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
}

void loop()
{
  long Entfernung = 0;
  
  // Sender kurz ausschalten um Störungen des Signal zu vermeiden
  digitalWrite(SENDEN, LOW);
  delay(5);

  // Signal senden
  digitalWrite(SENDEN, HIGH);
  delayMicroseconds(10);
  digitalWrite(SENDEN, LOW);
  
  // pulseIn -> Zeit messen, bis das Signal zurückkommt
  long Zeit = pulseIn(ECHO, HIGH);

  // Entfernung in cm berechnen
  Entfernung = (Zeit / 2) * 0.03432;

  // Entfernung anzeigen
  Serial.print("Entfernung in cm: ");
  Serial.print(Entfernung);
  Serial.print("\tZeit: ");
  Serial.println(Zeit);
  delay(1000);
}