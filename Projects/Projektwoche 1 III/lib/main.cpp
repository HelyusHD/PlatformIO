#include <Arduino.h>
#include <LiquidCrystal_I2C.h> // Library for LCD


char data = 0;            // Variable for storing received data
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

void setup()
{
    Serial.begin(9600);   // Sets the baud for serial data transmission                               
    pinMode(13, OUTPUT);  // Sets digital pin 13 as output pin
    lcd.init(); //initialize the lcd
    lcd.backlight(); //open the backlight 
    lcd.print("karl das toetet");
    lcd.setCursor(0, 1);
    lcd.print("Leute");
}

void loop()
{
   if(Serial.available() > 0)  // Send data only when you receive data
   {
      //data = Serial.read();    // Read the incoming data & store into data
      String buffer = Serial.readStringUntil('\n');
      data = buffer.c_str()[0];
      Serial.print("Received: ");  
      Serial.println(data);     // Print received value

      if (data == '1') {
         digitalWrite(13, HIGH);   // If value is 1 then LED turns ON
         Serial.println("LED is ON"); // Confirmation message
          lcd.clear();                 // clear display
          lcd.setCursor(0, 0);         // move cursor to   (0, 0)
          lcd.print("ON");        // print message at (0, 0)
      } 
      else if (data == '0') {
          digitalWrite(13, LOW);    // If value is 0 then LED turns OFF
          Serial.println("LED is OFF"); // Confirmation message
          lcd.clear();                 // clear display
          lcd.setCursor(0, 0);         // move cursor to   (0, 0)
          lcd.println("OFF"); // Confirmation message
      } 
      else {
          lcd.clear();                 // clear display
          lcd.setCursor(0, 0);
          lcd.print(buffer);
          Serial.println(buffer);
      }
   }
}
