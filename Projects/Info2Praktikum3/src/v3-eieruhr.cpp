#include <Arduino.h>
#include <LiquidCrystal_I2C.h>


#define GREEN_PIN 8
#define YELLOW_PIN 9
#define RED_PIN 10
#define Button_LEFT_PIN 11
#define Button_RIGHT_PIN 12
#define TICKS_PER_SEC 150000/2

// Neues Modul erzeugen: I2C-Adresse 0x27, 16 Stellen, 2 Zeilen 
LiquidCrystal_I2C lcd(0x27, 16, 2);  

void signal_light(int s);
bool update_state(int state, int stateLast, LiquidCrystal_I2C adress);
bool flankHigh(bool now, bool last);
String nice_time(int sec);
void beeping();

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
bool leftButtonPressed = false;
bool beepingLast = true;
int blinkingCounter=0;


void loop() {
  if(!digitalRead(Button_LEFT_PIN)){
    state = 15;
    digitalWrite(RED_PIN,HIGH);
    digitalWrite(YELLOW_PIN,LOW);
    digitalWrite(GREEN_PIN,LOW);
    beepingLast = false;
  }
  if(!digitalRead(Button_RIGHT_PIN)){
    state = 320;
    digitalWrite(RED_PIN,LOW);
    digitalWrite(YELLOW_PIN,LOW);
    digitalWrite(GREEN_PIN,HIGH);
    beepingLast = true;
  }
if(counter >= TICKS_PER_SEC*a){
    //Serial.print("state: ");
    //Serial.println(state);
    if (update_state(state,stateLast,lcd)){stateLast=state;}
        if(state > 0){
            state--;
        }else{
            if (flankHigh(true,beepingLast)){beeping();beepingLast=true;}
            if (blinkingCounter == 5){
                digitalWrite(RED_PIN,HIGH);
                digitalWrite(YELLOW_PIN,HIGH);
                digitalWrite(GREEN_PIN,HIGH);
            }
            if (blinkingCounter >= 10){
                digitalWrite(RED_PIN,LOW);
                digitalWrite(YELLOW_PIN,LOW);
                digitalWrite(GREEN_PIN,LOW);
                blinkingCounter = 0;
            }
            blinkingCounter++;
            Serial.print(blinkingCounter);
        }
        counter = 0;
    }else{
        counter++;
    }
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
    adress.print(nice_time(state));
    adress.print("   ");
    return true;
  }
  return false;
}

// returns true if high flank detected
bool flankHigh(bool now, bool last){
    if(now and !last){
        last = true;
        return true;
    }
    if(!now and last){last = false;}
    return false;
}

String nice_time(int seconds) {
  int min = seconds / 60;
  int sec = seconds % 60;

  String result = "";
  if (min < 10) result += "0";
  result += String(min);
  result += ":";
  if (sec < 10) result += "0";
  result += String(sec);

  return result;
}

void beeping(){
    tone(A0,500,5000);
}