#define USED_PIN 8

#include <Arduino.h>
#include <math.h>
int timer;
void setup() 
{ 
  pinMode(USED_PIN, OUTPUT);
  timer = 0;
} 
 
void loop()
{
  // Wir lassen die LED immer sehr schnell blinken und verändern das Verhältnis zwichen der an-/aus-Zeit
  // Damit lässt sich die Helligkeit verändern, da das Blinken so schnell ist, dass man eine Mittlung des Lichtes sieht.
  // Bei einem jeweiligen delay von 10ms ist der Effekt deutlich ersichtlich.
  // Wir haben uns den Spaß erlaubt, dass delay zeitlich zu verändern.
  timer++;
  digitalWrite(USED_PIN, HIGH); 
  delay(1);  
  digitalWrite(USED_PIN, LOW); 
  delay(timer/100 + 1);
  if(timer>=1000){timer = 0;}
} 




