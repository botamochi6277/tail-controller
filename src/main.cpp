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

// pin assignment
const uint8_t SERVO_H_ID = 5;
const uint8_t SERVO_V_ID = 6;

const uint8_t RX_D = 26;
const uint8_t TX_D = 32;

const uint8_t MIC_PIN = 33;     // pin no. for microphone
const uint8_t SW_SCL_PIN = 21;  // software i2c pins
const uint8_t SW_SDA_PIN = 25;
const uint8_t SPEAKER_PIN = 23;  // pin no. for piezo speaker
const uint8_t LED_PIN = 27;
botalab::Imu my_imu(SW_SDA_PIN, SW_SCL_PIN);  // sda, scl. mpu6050
const uint8_t NUM_LEDS = 1;
static CRGB leds[NUM_LEDS];

#endif

botalab::TailService tail_service;

unsigned long clock_ms;
unsigned short cmd = 0;
// imu reaction parameters
unsigned long last_fire_ms = 0;
const unsigned int imu_fire_interval = 2000;  // msec
const float imu_sq_fire_val = 3.0f * 9.81f * 9.81f;
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
  Serial1.begin(1000000, SERIAL_8N1, RX_D, TX_D);
  tail.init(&Serial1);
  if (Serial1) {
    Serial.printf("Serial1(%d, %d) is ready\n\r", RX_D, TX_D);
  }

#endif

  tail.attachServoIds(SERVO_H_ID, SERVO_V_ID);

  // led
  FastLED.addLeds<WS2812B, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  leds[0] = CHSV(170, 255, 255);  // blue
  FastLED.show();

  botalab::setupBle(tail_service);

  Tasks.add("imu_task", [] { my_imu.update(); })->startFps(100);
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
  Tasks
      .add("tail-update",
           [] {
             bool is_fired = false;
             if (my_imu.accSqrt() > imu_sq_fire_val) {
               if (clock_ms - last_fire_ms > imu_fire_interval) {
                 is_fired = true;
                 last_fire_ms = clock_ms;
                 Serial.printf("fire w/ IMU, %0.2f \r\n", my_imu.accSqrt());
               }
             }
             tail.update(cmd, is_fired);
           })
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
