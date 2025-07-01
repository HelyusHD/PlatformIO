#include <Arduino.h>
// LiquidCrystal_I2C Bibliothek einbinden 
#include <LiquidCrystal_I2C.h>       
 
// Neues Modul erzeugen: I2C-Adresse 0x27, 16 Stellen, 2 Zeilen 
LiquidCrystal_I2C lcd(0x27, 16, 2);  
 
void setup() { 
  lcd.init();      // LCD initialisieren (starten)  
  lcd.backlight(); // Hintergrundbeleuchtung einschalten 
} 
 
void loop() { 
  lcd.setCursor(0, 0); // Position des ersten Zeichens festlegen.  
                       // (0,0) ist das erste Zeichen in der ersten Zeile.  
 
  lcd.print("Praktikum Mikro"); 
 
  lcd.setCursor(0, 1); // (0,1) ist das erste Zeichen in der zweiten Zeile. 
  
  lcd.print("Viel Erfolg!");  
}