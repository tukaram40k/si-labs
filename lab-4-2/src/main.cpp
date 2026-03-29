#include <Arduino.h>
#include <Servo.h>

Servo myServo;  // Create servo object

void setup() {
  myServo.attach(7);  // Connects the servo on pin 9 to the servo object
}

void loop() {
  myServo.write(0);   // Move to 0 degrees
  delay(1000);        // Wait 1 second
  myServo.write(180); // Move to 180 degrees
  delay(1000);
}