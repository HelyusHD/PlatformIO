// 3.3% (used 1062 bytes from 32256 bytes)
// mit besserem Lauflicht
// 3.0% (used 956 bytes from 32256 bytes)

#include <Arduino.h>
#include <math.h>
int time;

/*void lauflicht(int d){
  digitalWrite(8,HIGH);
  delay(d);
  digitalWrite(8,LOW);
  digitalWrite(9,HIGH);
  delay(d);
  digitalWrite(9,LOW);
  digitalWrite(10,HIGH);
  delay(d);
  digitalWrite(10,LOW);
  digitalWrite(11,HIGH);
  delay(d);
  digitalWrite(11,LOW);
}
*/
void besseres_lauflicht(int d){
  for(int pos=8; pos<=11; pos++){
    digitalWrite(pos,HIGH);
    delay(d);
    digitalWrite(pos,LOW);
  }

}

void setup() 
{ 
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);  
  time = 50;
} 
 
void loop(){
  besseres_lauflicht(time);
}
 




