#include <Arduino.h>
#include <Servo.h>

#define SERVO_1_PIN 8
#define SERVO_2_PIN 9
int degree_0_servo_1 = 500;     // in microseconds
int degree_180_servo_1 = 6500;   // in microseconds
int degree_0_servo_2 = 550;     // in microseconds
int degree_180_servo_2 = 6500;   // in microseconds
char buffer[10] = "90";
long int clock = 0;
long int clockMax = 10000;
int counter = 0;

// put function declarations here:
void calibratedServo(int degree);

Servo myServo_1;
Servo myServo_2;

void setup() {  
  myServo_1.attach(SERVO_1_PIN, degree_0_servo_1, degree_180_servo_1);
  myServo_2.attach(SERVO_2_PIN, degree_0_servo_2, degree_180_servo_2);
  Serial.begin(9600);
  Serial.println(clockMax);
}  

void loop() {
  if(clock++ > clockMax){
    clock = 0;
    counter++;
    //Serial.println(counter);
    int angle = counter%360;
    if(angle > 180){
      angle = 360-angle;
    }
    calibratedServo(angle);
  }
  
} 

void calibratedServo(int degree){
  //Serial.readBytesUntil("\n", buffer, 10);  
  Serial.println(degree);
  myServo_1.write(degree);
  myServo_2.write(degree);
}