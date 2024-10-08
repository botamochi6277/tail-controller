
/**
 * @file tail_util.hpp
 * @author botamochi6277 (botamochi6277@icloud.com)
 * @brief
 * @version 0.1
 * @date 2021-08-07
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef TAIL_UTILS_HPP
#define TAIL_UTILS_HPP

#include <Arduino.h>
#include <STSServoDriver.h>

#include "Signals.hpp"

namespace botalab {

enum DIRECTION { UP, DOWN, RIGHT, LEFT };

unsigned int degToByteAngle(int angle_deg) {
  return map(angle_deg, -180, 180, 0, 4095);
}

class Tail {
 private:
  STSServoDriver servo_driver_;
  byte servo_h_id_;
  byte servo_v_id_;
  int angle_h_;  // cache
  int angle_v_;  // cache
  int amp_h_ = 60;
  int amp_v_ = 0;
  int duration_ = 5000;  // msec
  int elapsed_time_ = 0;
  bool is_swinging_ = false;

 public:
  Tail(/* args */);
  ~Tail();
  bool init(HardwareSerial *serialPort);
  void attachServoIds(int id_h, int id_v);
  void beginSwing(int amp_h, int amp_v, int speed);
  void beginRandomSwing();
  // nerve firing
  bool update(uint8_t cmd, bool is_fired);  // update state
  bool isSwinging();
  int inline angleH() { return this->angle_h_; }
  int inline angleV() { return this->angle_v_; }
};

Tail::Tail(/* args */) {}

Tail::~Tail() {}

bool Tail::init(HardwareSerial *serialPort) {
  return servo_driver_.init(serialPort);
}

void Tail::attachServoIds(int id_h, int id_v) {
  servo_h_id_ = id_h;
  servo_v_id_ = id_v;
}

void Tail::beginSwing(int amp_h, int amp_v, int speed) {
  if (!servo_driver_.isMoving(servo_h_id_)) {
    servo_driver_.setTargetPosition(servo_h_id_, degToByteAngle(amp_h), speed);
  }

  if (!servo_driver_.isMoving(servo_v_id_)) {
    servo_driver_.setTargetPosition(servo_v_id_, degToByteAngle(amp_v), speed);
  }
}

void Tail::beginRandomSwing() {
  int min_amp = 60;
  int max_amp = 90;
  // -90 -- -60, 60 -- 90
  int m_h = random(-max_amp, max_amp);
  int m_v = random(-max_amp, max_amp);

  int min_speed = 500;
  int max_speed = 1000;
  int speed = random(min_speed, max_speed);
  this->beginSwing(m_h, m_v, speed);
}

bool Tail::isSwinging() {
  return this->servo_driver_.isMoving(servo_h_id_) |
         this->servo_driver_.isMoving(servo_v_id_);
}

bool Tail::update(uint8_t cmd, bool is_fired) {
  switch (cmd) {
    case 0x01:
      this->beginSwing(90, 0, 500);
      break;
    case 0x02:
      this->beginSwing(-90, 0, 500);
      break;
    case 0x03:
      this->beginSwing(0, 90, 500);
      break;
    case 0x04:
      this->beginSwing(0, -90, 500);
      break;

    default:
      break;
  }

  if (!this->isSwinging() && is_fired) {
    this->beginRandomSwing();
  }

  // TODO: whim swinging

  return this->isSwinging();
}

int average(int *values, int length) {
  int sum = 0;
  for (int i = 0; i < length; i++) {
    sum += values[i];
  }
  return sum / length;
}

void pushBack(int *values, int length, int new_value) {
  for (int i = length - 1; i > 0; i--) {
    values[i] = values[i - 1];
  }
  values[0] = new_value;
}

// TODO: microphone

}  // namespace botalab
#endif