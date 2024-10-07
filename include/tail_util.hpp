
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
#include <LSM6DS3.h>
#include <STSServoDriver.h>

#include "signal.hpp"

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
  void update(int step);
  bool inline isSwinging() { return this->is_swinging_; }
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
  int m_h = random(min_amp, max_amp);
  int m_v = random(min_amp, max_amp);

  int min_speed = 500;
  int max_speed = 1000;
  int speed = random(min_speed, max_speed);
  this->beginSwing(m_h, m_v, speed);
}

class MyImu {
 private:
  LSM6DS3 device_;
  float acc_x_;
  float acc_y_;
  float acc_z_;
  float gyro_x_;
  float gyro_y_;
  float gyro_z_;

 public:
  MyImu(uint8_t bus_type, uint8_t address);
  inline float accX() { return acc_x_; }
  inline float accY() { return acc_y_; }
  inline float accZ() { return acc_z_; }
  inline float gyroX() { return gyro_x_; }
  inline float gyroY() { return gyro_y_; }
  inline float gyroZ() { return gyro_z_; }
  inline float accSqrt() {
    return acc_x_ * acc_x_ + acc_y_ * acc_y_ + acc_z_ * acc_z_;
  }

  void update();
  status_t begin() { return device_.begin(); }
};

MyImu::MyImu(uint8_t bus_type, uint8_t address) : device_(bus_type, address) {}

void MyImu::update() {
  acc_x_ = device_.readFloatAccelX();  // g
  acc_y_ = device_.readFloatAccelY();
  acc_z_ = device_.readFloatAccelZ();
  gyro_x_ = device_.readFloatGyroX();  // deg/sec
  gyro_y_ = device_.readFloatGyroY();  // deg/sec
  gyro_z_ = device_.readFloatGyroZ();  // deg/sec
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

#endif