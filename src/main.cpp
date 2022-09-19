#include <Arduino.h>
#include <Servo.h>
#include <TaskManager.h>

#include "LSM6DS3.h"
#include "Wire.h"
#include "tail_util.hpp"

Tail tail;

MyImu my_imu(I2C_MODE, 0x6A);  // I2C device address 0x6A
int imu_freq = 10;

void SweepUpdate(Servo &h, Servo &v);

unsigned int wait_time = 10;

void setup() {
  // put your setup code here, to run once:
  // build-in leds
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  // turn off the all build-in leds
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  Serial.begin(9600);
  if (my_imu.begin() != 0) {
    Serial.println("Device error");
    digitalWrite(LEDR, LOW);
  } else {
    Serial.println("Device OK!");
  }

  tail.attachPin(4, 5);

  Tasks.add("imu_task", [] { my_imu.update(); })->startFps(100);
  Tasks.add("tail", [] { tail.update(100); })->startFps(10);

  delay(3000);
}

void loop() {
  Tasks.update();  // automatically execute tasks

  if (tail.isSwinging() == false) {
    if (my_imu.accSqrt() > 2.0f) {
      tail.beginRandomSwing();
    }
  }
}
