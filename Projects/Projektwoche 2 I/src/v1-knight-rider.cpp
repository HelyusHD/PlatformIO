#include <Arduino.h>

void besseres_lauflicht_links(int d){
    for(int pos=8; pos<=11; pos++){
      digitalWrite(pos,HIGH);
      delay(d);
      digitalWrite(pos,LOW);
    }
}
void besseres_lauflicht_rechts(int d){
    for(int pos=11; pos>=8; pos--){
      digitalWrite(pos,HIGH);
      delay(d);
      digitalWrite(pos,LOW);
    }
  
}
void knight_rider(int d){
    besseres_lauflicht_links(d);
    besseres_lauflicht_rechts(d);
}
void alle_an(){
    for(int pos=8; pos<=11; pos++){
        digitalWrite(pos,HIGH);
      }
}
void alle_aus(){
    for(int pos=8; pos<=11; pos++){
        digitalWrite(pos,LOW);
      }
}


void setup() 
{ 
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);  
} 
 

void loop()
{    
    if (!digitalRead(7) && !digitalRead(6)) { // Taste 1 & Taste 2 gedrueckt?
    alle_an();
}
    else if (!digitalRead(7)) { // Taste 1 gedrueckt? - links/rechts
        besseres_lauflicht_links(50);
    }
    else if (!digitalRead(6)) { // Taste 2 gedrueckt? - rechts/links
        besseres_lauflicht_rechts(50);
    }
    else if (!digitalRead(5)) { // Taste 3 gedrueckt?
        knight_rider(50);
    }

    else {alle_aus();}
}


