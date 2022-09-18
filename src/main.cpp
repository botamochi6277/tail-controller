#include <Arduino.h>
#include <Servo.h>
#include "tail_util.hpp"

#include "LSM6DS3.h"
#include "Wire.h"

Tail tail;

MyImu my_imu(I2C_MODE, 0x6A); // I2C device address 0x6A

void SweepUpdate(Servo &h, Servo &v);

unsigned int wait_time = 10;

void setup()
{
  // put your setup code here, to run once:
  // build-in leds
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  // turn off the all build-in leds
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  tail.attachPin(4, 5);
  delay(3000);
}

void loop()
{

  if (!tail.isSwinging())
  {
    tail.beginRandomSwing();
  }

  tail.update(wait_time);
  delay(wait_time);
}
