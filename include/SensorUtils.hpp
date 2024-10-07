#ifndef SENSOR_UTILS_CPP
#define SENSOR_UTILS_CPP
#include <Arduino.h>
#include <LSM6DS3.h>
namespace botalab {

class Imu {
 private:
  LSM6DS3 device_;
  float acc_x_;
  float acc_y_;
  float acc_z_;
  float gyro_x_;
  float gyro_y_;
  float gyro_z_;

 public:
  Imu(uint8_t bus_type, uint8_t address);
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

Imu::Imu(uint8_t bus_type, uint8_t address) : device_(bus_type, address) {}

void Imu::update() {
  acc_x_ = device_.readFloatAccelX();  // g
  acc_y_ = device_.readFloatAccelY();
  acc_z_ = device_.readFloatAccelZ();
  gyro_x_ = device_.readFloatGyroX();  // deg/sec
  gyro_y_ = device_.readFloatGyroY();  // deg/sec
  gyro_z_ = device_.readFloatGyroZ();  // deg/sec
}
}  // namespace botalab

#endif