/*
 * MLR - Mini line follower robot: charging mode
 * https://github.com/marvinramm/MLR-Mini-Line-Follower-Robot
 *
 * Copyright (c) 2024 Marvin Ramm
 * SPDX-License-Identifier: MIT
 *
 * Board: "ESP32C3 Dev Module", USB CDC On Boot: "Enabled"
 *
 * Minimal sketch for charging over USB-C: motors stay off, the RGB LEDs
 * are switched off.
 */

#include "Pins.h"

bool ShiftRegisterBits[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void ShiftRegisterWrite(int pin, bool state) {
  if (ShiftRegisterBits[pin] != state) {
    ShiftRegisterBits[pin] = state;
    digitalWrite(STCP, LOW);
    for (int i = 7; i >= 0; i--) {
      digitalWrite(SHCP, LOW);
      digitalWrite(DS, ShiftRegisterBits[i]);
      digitalWrite(SHCP, HIGH);
    }
    digitalWrite(STCP, HIGH);
  }
}

void setup() {
  pinMode(SHCP, OUTPUT);
  pinMode(STCP, OUTPUT);
  pinMode(DS, OUTPUT);
  pinMode(WHITE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);

  digitalWrite(WHITE, HIGH);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);

  ShiftRegisterWrite(SR_RED, HIGH);
  ShiftRegisterWrite(SR_GREEN, HIGH);
}

void loop() {
}
