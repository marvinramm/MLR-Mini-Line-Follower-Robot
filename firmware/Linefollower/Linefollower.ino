/*
 * MLR - Mini line follower robot: line follower
 * https://github.com/marvinramm/MLR-Mini-Line-Follower-Robot
 *
 * Copyright (c) 2024 Marvin Ramm
 * SPDX-License-Identifier: MIT
 *
 * Board: "ESP32C3 Dev Module", USB CDC On Boot: "Enabled"
 *
 * Follows a black line with the two phototransistors under the robot.
 * The difference between the right and the left sensor is added to one
 * motor side and subtracted from the other (proportional control).
 */

#include "Pins.h"

bool ShiftRegisterBits[8] = {0, 0, 0, 0, 0, 0, 0, 0};

#define L 0
#define R 1

#define OUT_A 1
#define OUT_B 2
#define OUT_AB 3

float FollowLineDiffFactor = 0.5;

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

void ShiftRegisterReset() {
  digitalWrite(STCP, LOW);
  for (int i = 7; i >= 0; i--) {
    digitalWrite(SHCP, LOW);
    digitalWrite(DS, 0);
    digitalWrite(SHCP, HIGH);
  }
  digitalWrite(STCP, HIGH);
}

void Forward(byte mot, int v) {
    v = constrain(v, -240, 240);
  ShiftRegisterWrite(SR_STBY, HIGH);
  if (v >= 0) {
    if (mot & OUT_A) {
      ShiftRegisterWrite(SR_AIN1, LOW);
      ShiftRegisterWrite(SR_AIN2, HIGH);
      analogWrite(PWMA, v);
    }
    if (mot & OUT_B) {
      ShiftRegisterWrite(SR_BIN1, LOW);
      ShiftRegisterWrite(SR_BIN2, HIGH);
      analogWrite(PWMB, v);
    }
  } else {
    v = -v;
    if (mot & OUT_A) {
      ShiftRegisterWrite(SR_AIN1, HIGH);
      ShiftRegisterWrite(SR_AIN2, LOW);
      analogWrite(PWMA, v);
    }
    if (mot & OUT_B) {
      ShiftRegisterWrite(SR_BIN1, HIGH);
      ShiftRegisterWrite(SR_BIN2, LOW);
      analogWrite(PWMB, v);
    }
  }
}

void Backward(byte mot, int v) {  //Turns a certain motor backward at a certain speed
  Forward(mot, -v);
}

void Stop() {
  ShiftRegisterWrite(SR_STBY, LOW);
  ShiftRegisterWrite(SR_AIN1, LOW);
  ShiftRegisterWrite(SR_AIN2, LOW);
  ShiftRegisterWrite(SR_BIN1, LOW);
  ShiftRegisterWrite(SR_BIN2, LOW);
}

void FollowLine(int v) {
  int diff = analogRead(PT_R) - analogRead(PT_L);
  if (abs(diff) < 10) diff = 0;
  int vl = v + diff;
  int vr = v - diff;
  Forward(OUT_B, vl);
  Forward(OUT_A, vr);
}

void setup() {
  Serial.begin(115200);
  Serial.println("MLR Line follower");

  analogReadResolution(9); //12 - default, 9 - faster ADC

  // Outputs
  pinMode(SHCP, OUTPUT);
  pinMode(STCP, OUTPUT);
  pinMode(DS, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(WHITE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);

  //Inputs
  pinMode(PT_L, INPUT);
  pinMode(PT_R, INPUT);

  ShiftRegisterReset();

  digitalWrite(WHITE, LOW);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);

  ShiftRegisterWrite(SR_RED, HIGH);
  ShiftRegisterWrite(SR_GREEN, HIGH);

  delay(2000);

  ShiftRegisterWrite(PWR_EN, HIGH);
}

void loop() {
   FollowLine(80);
}
