#include <Arduino.h>

// Define the LED pin for external LED connection
#define LED_PIN 7  // Using pin 7 for external LED

void setup() {
  // Initialize the LED pin as an output
  pinMode(LED_PIN, OUTPUT);
  
  // Initialize serial communication
  Serial.begin(9600);
  
  // Turn off the LED initially
  digitalWrite(LED_PIN, LOW);
  
  // Print startup message
  Serial.println("External LED Controller Ready");
  Serial.println("Send 'LED ON' to turn LED on, 'LED OFF' to turn LED off");
}

void loop() {
  // Check if there's incoming data on the serial port
  if (Serial.available() > 0) {
    // Read the incoming string
    String command = Serial.readString();
    
    // Remove any trailing whitespace/newline characters
    command.trim();
    
    // Convert to uppercase for easier comparison
    command.toUpperCase();
    
    // Check the command and control the LED accordingly
    if (command == "LED ON") {
      digitalWrite(LED_PIN, HIGH);  // Turn LED on
      Serial.println("External LED turned ON");
    } else if (command == "LED OFF") {
      digitalWrite(LED_PIN, LOW);   // Turn LED off
      Serial.println("External LED turned OFF");
    } else {
      Serial.println("Unknown command. Use 'LED ON' or 'LED OFF'");
    }
  }
}