/*
 * MLR - Mini line follower robot: keep heading with the IMU
 * https://github.com/marvinramm/MLR-Mini-Line-Follower-Robot
 *
 * Copyright (c) 2024 Marvin Ramm
 * SPDX-License-Identifier: MIT
 *
 * Board: "ESP32C3 Dev Module", USB CDC On Boot: "Enabled"
 *
 * Fuses gyro and accelerometer of the LSM6DS3TR-C with a Madgwick filter
 * and turns the robot back to its start heading when it is rotated.
 * Tilting the robot by more than about 60 degrees switches it off.
 * Requires the FastIMU library (https://github.com/LiquidCGS/FastIMU).
 */

#include "Pins.h"

#include "FastIMU.h"
#include "Madgwick.h"
#include <Wire.h>

#define IMU_ADDRESS 0x6B  //Change to the address of the IMU
//#define PERFORM_CALIBRATION //Comment to disable startup calibration
LSM6DS3 IMU;  //Change to the name of any supported IMU!

calData calib = { 0 };  //Calibration data
AccelData IMUAccel;     //Sensor data
GyroData IMUGyro;
MagData IMUMag;
Madgwick filter;

bool ShiftRegisterBits[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

#define L 0
#define R 1

#define OUT_A 1
#define OUT_B 2
#define OUT_AB 3

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

void setup() {
  Serial.begin(115200);
  Serial.println("MLR-KeepAngle");

  Wire.begin(21, 20);
  Wire.setClock(400000);  //400khz clock

  analogReadResolution(9);  //12 - default, 9 - faster ADC

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

  digitalWrite(WHITE, HIGH);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, HIGH);

  ShiftRegisterWrite(SR_RED, HIGH);
  ShiftRegisterWrite(SR_GREEN, HIGH);

  delay(1500);

  ShiftRegisterWrite(PWR_EN, HIGH);

  ShiftRegisterWrite(SR_RED, HIGH);
  ShiftRegisterWrite(SR_GREEN, LOW);

  delay(500);

  int err = IMU.init(calib, IMU_ADDRESS);
  if (err != 0) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
    while (true) {
      ;
    }
  }

#ifdef PERFORM_CALIBRATION
  Serial.println("FastIMU Calibrated Quaternion example");
  if (IMU.hasMagnetometer()) {
    delay(1000);
    Serial.println("Move IMU in figure 8 pattern until done.");
    delay(3000);
    IMU.calibrateMag(&calib);
    Serial.println("Magnetic calibration done!");
  } else {
    delay(1000);
  }
  Serial.println("Keep IMU level.");
  IMU.calibrateAccelGyro(&calib);
  Serial.println("Calibration done!");
  Serial.println("Accel biases X/Y/Z: ");
  Serial.print(calib.accelBias[0]);
  Serial.print(", ");
  Serial.print(calib.accelBias[1]);
  Serial.print(", ");
  Serial.println(calib.accelBias[2]);
  Serial.println("Gyro biases X/Y/Z: ");
  Serial.print(calib.gyroBias[0]);
  Serial.print(", ");
  Serial.print(calib.gyroBias[1]);
  Serial.print(", ");
  Serial.println(calib.gyroBias[2]);
  if (IMU.hasMagnetometer()) {
    Serial.println("Mag biases X/Y/Z: ");
    Serial.print(calib.magBias[0]);
    Serial.print(", ");
    Serial.print(calib.magBias[1]);
    Serial.print(", ");
    Serial.println(calib.magBias[2]);
    Serial.println("Mag Scale X/Y/Z: ");
    Serial.print(calib.magScale[0]);
    Serial.print(", ");
    Serial.print(calib.magScale[1]);
    Serial.print(", ");
    Serial.println(calib.magScale[2]);
  };
  IMU.init(calib, IMU_ADDRESS);

  filter.begin(0.2f);
#endif

  ShiftRegisterWrite(SR_RED, LOW);
  ShiftRegisterWrite(SR_GREEN, LOW);

}

void IMUUpdate() {
  IMU.update();
  IMU.getAccel(&IMUAccel);
  IMU.getGyro(&IMUGyro);
  if (IMU.hasMagnetometer()) {
    IMU.getMag(&IMUMag);
    filter.update(IMUGyro.gyroX, IMUGyro.gyroY, IMUGyro.gyroZ, IMUAccel.accelX, IMUAccel.accelY, IMUAccel.accelZ, IMUMag.magX, IMUMag.magY, IMUMag.magZ);
  } else {
    filter.updateIMU(IMUGyro.gyroX, IMUGyro.gyroY, IMUGyro.gyroZ, IMUAccel.accelX, IMUAccel.accelY, IMUAccel.accelZ);
  }
}

void IMUAllign(float y){
  if (filter.getQuatZ() < y){
    if (abs(filter.getQuatZ()) < y + 0.05){
    Stop();
      }else{
        Forward(OUT_A, 150);
        Backward(OUT_B, 150);
      }
  }else{
    if (abs(filter.getQuatZ()) < y + 0.05){
    Stop();
      }else{
        Forward(OUT_B, 150);
        Backward(OUT_A, 150);
      }
  }
}

void PrintIMU() {
  Serial.print("QW: ");
  Serial.print(filter.getQuatW());
  Serial.print("\tQX: ");
  Serial.print(filter.getQuatX());
  Serial.print("\tQY: ");
  Serial.print(filter.getQuatY());
  Serial.print("\tQZ: ");
  Serial.println(filter.getQuatZ());
  delay(50);
}

void loop() {

  
  IMUUpdate();

  while (abs(filter.getQuatY()) <= 0.50) {
    IMUUpdate();
    //PrintIMU();
    IMUAllign(0.00);
  }
  ShiftRegisterWrite(SR_RED, LOW);
  ShiftRegisterWrite(SR_GREEN, HIGH);
  Stop();
  delay(1500);
  ShiftRegisterWrite(PWR_EN, LOW);
}
