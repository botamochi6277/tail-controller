#include <Arduino.h>
#include <ArduinoBLE.h>
#include <LSM6DS3.h>
#include <TaskManager.h>
#include <Wire.h>

#include "MiscUtils.hpp"
#include "tail_util.hpp"
Tail tail;
#ifdef TARGET_LIKE_MBED
arduino::UART my_serial(7, 8);
#endif
MyImu my_imu(I2C_MODE, 0x6A);  // I2C device address 0x6A
int imu_freq = 10;

// BLE Variables
BLEService tail_service("70882CA0-6C07-426C-93F8-5C9483D06360");
// clock Characteristic
BLEUnsignedLongCharacteristic timer_characteristic(
    "70882CA1-6C07-426C-93F8-5C9483D06360", BLERead | BLENotify);
BLEDescriptor timer_descriptor("2901", "timer_ms");

BLEUnsignedShortCharacteristic command_characteristic(
    "70882CA2-6C07-426C-93F8-5C9483D06360", BLERead | BLEWrite);
BLEDescriptor cmd_descriptor("2901", "command");

unsigned int wait_time = 10;

unsigned long clock_ms;
unsigned short cmd = 0;
bool cmd_written = false;

void bleTask();
void SwingEventTask();
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

  tail_service.addCharacteristic(timer_characteristic);
  timer_characteristic.addDescriptor(timer_descriptor);

  tail_service.addCharacteristic(command_characteristic);
  command_characteristic.addDescriptor(cmd_descriptor);

  BLE.addService(tail_service);

  BLE.advertise();
}

void setup() {
  // put your setup code here, to run once:

#if defined(LEDR)
  botalab::init_xiao_nrf52840_builtin_led();
#endif

  Serial.begin(9600);
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
  Serial1.begin(1000000, SERIAL_8N1, 7, 8);
  tail.init(&Serial1);
#endif

  tail.attachServoIds(5, 6);

  bleSetup();

  Tasks.add("imu_task", [] { my_imu.update(); })->startFps(100);
  // Tasks.add("tail", [] { tail.update(100); })->startFps(10);
  Tasks.add("ble", [&] { bleTask(); })->startFps(100);
  Tasks.add("event", [] { SwingEventTask(); })->startFps(100);
#if defined(LEDB)
  Tasks.add("heartbeat", [] { digitalWrite(LEDB, !digitalRead(LEDB)); })
      ->startFps(1);
#endif

  delay(3000);
}

void loop() {
  clock_ms = millis();
  Tasks.update();  // automatically execute tasks
}

void bleTask() {
  // poll for Bluetooth® Low Energy events
  BLE.poll();
  timer_characteristic.writeValueLE(clock_ms);
  if (command_characteristic.written()) {
    cmd = command_characteristic.valueLE();
    cmd_written = true;
  }
}

void SwingEventTask() {
  if (cmd_written) {
    // manual tail swing

    switch (cmd) {
      case 0x01:
        tail.beginSwing(90, 0, 500);
        break;
      case 0x02:
        tail.beginSwing(-90, 0, 500);
        break;
      case 0x03:
        tail.beginSwing(0, 90, 500);
        break;
      case 0x04:
        tail.beginSwing(0, -90, 500);
        break;

      default:
        break;
    }
    cmd_written = false;
  }
  if (tail.isSwinging() == false) {
    if (my_imu.accSqrt() > 2.0f) {
      tail.beginRandomSwing();
    }
  }
}