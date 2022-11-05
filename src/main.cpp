#include <Arduino.h>
#include <ArduinoBLE.h>
#include <LSM6DS3.h>
#include <Servo.h>
#include <TaskManager.h>
#include <Wire.h>

#include "tail_util.hpp"

Tail tail;

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
  Serial.println(BLE.address());

  // set advertised local name and service UUID:
  BLE.setDeviceName("HackaTail");
  BLE.setLocalName("SpermWhale");
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
    Serial.println("IMU Device error");
    digitalWrite(LEDR, LOW);
  } else {
    Serial.println("IMU Device OK!");
  }

  tail.attachPin(4, 5);

  bleSetup();

  Tasks.add("imu_task", [] { my_imu.update(); })->startFps(100);
  Tasks.add("tail", [] { tail.update(100); })->startFps(10);
  Tasks.add("ble", [&] { bleTask(); })->startFps(100);
  Tasks.add("event", [] { SwingEventTask(); })->startFps(100);

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
        tail.beginSwing(90, 0, 3000, 0);
        break;
      case 0x02:
        tail.beginSwing(-90, 0, 3000, 0);
        break;
      case 0x03:
        tail.beginSwing(0, 90, 3000, 0);
        break;
      case 0x04:
        tail.beginSwing(0, -90, 3000, 0);
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