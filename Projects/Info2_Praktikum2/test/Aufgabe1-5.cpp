#include <Arduino.h>

#define GREEN_PIN 8
#define YELLOW_PIN 9
#define RED_PIN 10
#define Button_LEFT_PIN 11
#define Button_RIGHT_PIN 12
#define DELAY 1000
#define DELAY_LONG 5000

void signal_light(int s){
  if(s <= 2){
    digitalWrite(RED_PIN,HIGH);
    digitalWrite(YELLOW_PIN,LOW);
  }else if(s <= 3){
    digitalWrite(YELLOW_PIN,HIGH);
  }else if(s <= 7){
    digitalWrite(RED_PIN,LOW);
    digitalWrite(YELLOW_PIN,LOW);
    digitalWrite(GREEN_PIN,HIGH);
  }else{
    digitalWrite(YELLOW_PIN,HIGH);
    digitalWrite(GREEN_PIN,LOW);
  }
}

void setup() {
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(Button_LEFT_PIN, INPUT_PULLUP);
  pinMode(Button_RIGHT_PIN, INPUT_PULLUP);

  digitalWrite(RED_PIN,LOW);
  digitalWrite(YELLOW_PIN,LOW);
  digitalWrite(GREEN_PIN,LOW);

  Serial.begin(9600); 
  Serial.println("terminal connected"); 
}

int state = 0;

void loop() {
  if(state < 10){
    state++;
  }else{
    state = 0;
  }
  if(!digitalRead(Button_RIGHT_PIN)){
    delay(DELAY_LONG);
  }else{
    delay(DELAY);
  }
  
  Serial.print("state: "); Serial.println(state);
  if(!digitalRead(Button_LEFT_PIN))
  { 
    digitalWrite(RED_PIN, HIGH); // LED an Pin 8 anschalten
    digitalWrite(YELLOW_PIN,LOW);
    digitalWrite(GREEN_PIN,LOW);
    state = 0;
  }else {
    digitalWrite(RED_PIN, LOW); 
  }
  signal_light(state);
}

