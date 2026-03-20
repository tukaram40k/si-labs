#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "../lib/IO/IO.h"
#include "../lib/LedDriver/LedController.h"

#include <IRremote.hpp>

#define IR_RECEIVE_PIN 2
#define ACTUATOR_PIN 7

void setup() {
  pinMode(ACTUATOR_PIN, OUTPUT);
  IrReceiver.begin(IR_RECEIVE_PIN);
}

void loop() {
  if (IrReceiver.decode()) {
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;

    if (code == 0xEA15FF00) {
      digitalWrite(ACTUATOR_PIN, HIGH);
    } 
    else if (code == 0xF807FF00) {
      digitalWrite(ACTUATOR_PIN, LOW);
    }

    IrReceiver.resume();
  }
}