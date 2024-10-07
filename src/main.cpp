#include <Arduino.h>
#include <ArduinoBLE.h>
#include <TaskManager.h>
#include <Wire.h>

#include "MiscUtils.hpp"
#include "SensorUtils.hpp"
#include "TailService.hpp"
#include "TailUtils.hpp"
botalab::Tail tail;
#ifdef TARGET_LIKE_MBED
arduino::UART my_serial(7, 8);

botalab::Imu my_imu(I2C_MODE, 0x6A);  // I2C device address 0x6A

#elif defined(ARDUINO_M5Stack_ATOM)

botalab::Imu my_imu(25, 21);  // sda, scl

#endif
int imu_freq = 10;

botalab::TailService tail_service;

unsigned int wait_time = 10;

unsigned long clock_ms;
unsigned short cmd = 0;
bool cmd_written = false;

void bleSetup() {
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    return;
  }
  Serial.print("Peripheral address: ");
  String ble_address = BLE.address();
  Serial.println(ble_address);

  String localname = "SpermWhale_";
  localname += ble_address;

  // set advertised local name and service UUID:
  BLE.setDeviceName("HackaTail");
  BLE.setLocalName(localname.c_str());
  BLE.setAdvertisedService(tail_service);
  BLE.addService(tail_service);
  BLE.advertise();
}

void setup() {
  // put your setup code here, to run once:

#if defined(LEDR)
  botalab::init_xiao_nrf52840_builtin_led();
#endif

  Serial.begin(115200);
  if (my_imu.begin() != 0) {
    Serial.println("IMU Device error");
#if defined(LEDR)
    digitalWrite(LEDR, LOW);
#endif
  } else {
    Serial.println("IMU Device OK!");
  }

  // serial for servos

#ifdef TARGET_LIKE_MBED
  tail.init(&my_serial);
#else
  // HardwareSerial
  Serial1.begin(1000000, SERIAL_8N1, 26, 32);
  tail.init(&Serial1);
  if (Serial1) {
    Serial.printf("Serial1(%d, %d) is ready\n\r", 26, 32);
  }

#endif

  tail.attachServoIds(5, 6);

  bleSetup();

  Tasks.add("imu_task", [] { my_imu.update(); })->startFps(100);
  // Tasks.add("tail", [] { tail.update(100); })->startFps(10);
  Tasks
      .add("ble",
           [&] {
             tail_service.update();
             if (tail_service.command_chr.written()) {
               cmd = tail_service.command_chr.read();
             } else {
               cmd = 0;
             }
           })
      ->startFps(100);
  Tasks.add("event", [] { tail.update(cmd, my_imu.accSqrt() > 2.0f * 9.81f); })
      ->startFps(10);
#if defined(LEDB)
  Tasks.add("heartbeat", [] { digitalWrite(LEDB, !digitalRead(LEDB)); })
      ->startFps(1);
#endif
  tail.beginSwing(0, 0, 1000);  // back to idling position
  Serial.println("setup was completed");
  delay(3000);
}

void loop() {
  clock_ms = millis();
  Tasks.update();  // automatically execute tasks
}
