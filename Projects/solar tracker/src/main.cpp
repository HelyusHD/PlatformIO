#include <Arduino.h>
#include <Servo.h>

#include <sunPosition.h>

#define SERVO_1_PIN 8
#define SERVO_2_PIN 9
int degree_0_servo_1 = 500;     // in microseconds
int degree_180_servo_1 = 2400;   // in microseconds
int degree_0_servo_2 = 600;     // in microseconds
int degree_180_servo_2 = 2400;   // in microseconds

char buffer[10] = "90";

long int clock = 0;
long int clockMax = 1000000;
int counter = 0;

PolarDirection direction;
Time time;

// put function declarations here:
void calibratedServo(PolarDirection direction);

Servo myServo_1;
Servo myServo_2;

void setup() {  
  myServo_1.attach(SERVO_1_PIN, degree_0_servo_1, degree_180_servo_1);
  myServo_2.attach(SERVO_2_PIN, degree_0_servo_2, degree_180_servo_2);
  Serial.begin(9600);
  Serial.println(clockMax);
  time.day = 3;
  time.hour = 14;
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
    direction = sunPosition(time);
    calibratedServo(direction);
  }
  
} 

void calibratedServo(PolarDirection direction){
  //Serial.readBytesUntil("\n", buffer, 10);
  Serial.println();
  float a = 0;
  direction.azimuth = a;
  direction.elevation = a;
  Serial.println(direction.azimuth);
  Serial.println(direction.elevation);
  myServo_1.write(direction.azimuth);
  myServo_2.write(direction.elevation);
}