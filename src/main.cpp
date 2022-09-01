#include <Arduino.h>
#include <Servo.h>
#include "tail_util.hpp"

Tail tail;

void SweepUpdate(Servo &h, Servo &v);

unsigned int wait_time = 10;

void setup()
{
  // put your setup code here, to run once:
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
