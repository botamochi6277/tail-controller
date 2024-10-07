#ifndef SENSOR_UTILS_CPP
#define SENSOR_UTILS_CPP
#include <Arduino.h>

#if defined(TARGET_LIKE_MBED)
#include <LSM6DS3.h>
#elif defined(ARDUINO_M5Stack_ATOM)
#include <Adafruit_MPU6050.h>
#endif

namespace botalab {

class Imu {
 private:
#ifdef __LSM6DS3IMU_H__
  LSM6DS3 device_;
#elif defined(ARDUINO_M5Stack_ATOM)
  Adafruit_MPU6050 device_;

  uint8_t sda_;
  uint8_t scl_;
#endif

  float acc_x_;
  float acc_y_;
  float acc_z_;
  float gyro_x_;
  float gyro_y_;
  float gyro_z_;

 public:
#ifdef __LSM6DS3IMU_H__
  Imu(uint8_t bus_type, uint8_t address);
  bool begin() { return device_.begin(); }

#elif defined(ARDUINO_M5Stack_ATOM)
  Imu(uint8_t sda, uint8_t scl);
  bool begin();
#endif

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
};

#ifdef __LSM6DS3IMU_H__

Imu::Imu(uint8_t bus_type, uint8_t address) : device_(bus_type, address) {}

#elif defined(ARDUINO_M5Stack_ATOM)
Imu::Imu(uint8_t sda, uint8_t scl) : sda_(sda), scl_(scl) {};
bool Imu::begin() {
  Wire.begin(sda_, scl_, 100000);
  return this->device_.begin(MPU6050_I2CADDR_DEFAULT, &Wire);
}

#endif

void Imu::update() {
#ifdef __LSM6DS3IMU_H__
  acc_x_ = device_.readFloatAccelX();  // g
  acc_y_ = device_.readFloatAccelY();
  acc_z_ = device_.readFloatAccelZ();
  gyro_x_ = device_.readFloatGyroX();  // deg/sec
  gyro_y_ = device_.readFloatGyroY();  // deg/sec
  gyro_z_ = device_.readFloatGyroZ();  // deg/sec
#elif defined(ARDUINO_M5Stack_ATOM)
  sensors_event_t a, g, temp;
  if (this->device_.getEvent(&a, &g, &temp)) {
    // linear_acceleration
    acc_x_ = a.acceleration.x;  // m/s^2
    acc_y_ = a.acceleration.y;
    acc_z_ = a.acceleration.z;

    // angular_velocity
    gyro_x_ = g.gyro.x;  // rad/s
    gyro_y_ = g.gyro.y;
    gyro_z_ = g.gyro.z;
  }
#endif
}
}  // namespace botalab

#endif