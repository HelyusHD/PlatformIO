#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define GREEN_PIN 8
#define YELLOW_PIN 9
#define RED_PIN 10
#define Button_LEFT_PIN 11
#define Button_RIGHT_PIN 12
#define TICKS_PER_SEC 150000/5

// Neues Modul erzeugen: I2C-Adresse 0x27, 16 Stellen, 2 Zeilen 
LiquidCrystal_I2C lcd(0x27, 16, 2);  

void signal_light(int s);
bool update_state(int state, int stateLast, LiquidCrystal_I2C adress);
bool flip_flop(bool flip);

void setup() {
  lcd.init();      // LCD initialisieren (starten)  
  lcd.backlight(); // Hintergrundbeleuchtung einschalten 
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
  Serial.println("Ampel krass");
}

int state = 0;
int stateLast = -1;
long int counter = 0;
int a = 1;
bool isFlipped = false;
bool leftButtonPressed = false;

void loop() {
  if(flip_flop(!digitalRead(Button_LEFT_PIN)))
  { 
    digitalWrite(RED_PIN, HIGH); // LED an Pin 8 anschalten
    digitalWrite(YELLOW_PIN,LOW);
    digitalWrite(GREEN_PIN,LOW);
    state = 0;
    update_state(state,stateLast,lcd);
  }else{
    if(counter >= TICKS_PER_SEC*a){
        //Serial.print("state: ");
        //Serial.println(state);
        if (update_state(state,stateLast,lcd)){stateLast=state;}
        if(state < 10){
            state++;
        }else{
            state = 0;
        }
        counter = 0;
    }else{
        counter++;
    }
    if(!digitalRead(Button_RIGHT_PIN))
    {
      a =2;
    }
    else {
      a=1;
    }
  }

  signal_light(state);
}

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

bool update_state(int state, int stateLast, LiquidCrystal_I2C adress){
  if(state != stateLast){
    adress.setCursor(0,0);
    adress.print("state is:");
    adress.setCursor(0,1);
    adress.print(state);
    adress.print("   ");
    return true;
  }
  return false;
}

bool flip_flop(bool flip){
  if (flip and !leftButtonPressed){
    isFlipped = !isFlipped;
    leftButtonPressed = true;
  }
  if (!flip and leftButtonPressed){leftButtonPressed = false;}

  //if (isFlipped){Serial.println("is flipped");}else{Serial.println("is not flipped");}

  return isFlipped;
}