
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

#ifndef TAIL_UTIL
#define TAIL_UTIL

#include <Arduino.h>
#include <Servo.h>
#include "signal.hpp"

enum DIRECTION
{
    UP,
    DOWN,
    RIGHT,
    LEFT
};

void initServo(Servo &servo, int pin)
{
    servo.attach(pin, 600, 2400);
    servo.write(90); // rotate to initial orientation
}

class Tail
{
private:
    Servo servo_h_;
    Servo servo_v_;
    int angle_h_;
    int angle_v_;
    int amp_h_ = 60;
    int amp_v_ = 0;
    int duration_ = 5000; // msec
    int elapsed_time_ = 0;
    bool is_swingwing_ = false;

public:
    Tail(/* args */);
    ~Tail();
    void attachPin(int pin_h, int pin_v);
    void beginSwing(int amp_h, int amp_v, int duration, int ease = 0);
    void beginRandomSwing();
    void update(int step);
    bool inline isSwinging() { return this->is_swingwing_; }
    int inline angleH() { return this->angle_h_; }
    int inline angleV() { return this->angle_v_; }
};

Tail::Tail(/* args */) {}

Tail::~Tail() {}

void Tail::beginSwing(int amp_h, int amp_v, int duration, int ease)
{
    this->amp_h_ = amp_h;
    this->amp_v_ = amp_v;
    this->duration_ = duration;
    this->elapsed_time_ = 0;
    this->is_swingwing_ = true;
}

void Tail::beginRandomSwing()
{
    int m = random(60, 80);
    float angle = random(0, 360) * PI / 180.0f;
    int h = m * cos(angle);
    int v = m * sin(angle);

    int t = random(2000, 3000);
    this->beginSwing(h, v, t);
}

void Tail::update(int step)
{
    if (this->is_swingwing_)
    {
        this->elapsed_time_ += step;
        this->angle_h_ = 90 + TrianglePulse(elapsed_time_, int(0.5 * duration_), this->amp_h_, duration_);
        this->angle_v_ = 90 + TrianglePulse(elapsed_time_, int(0.5 * duration_), this->amp_v_, duration_);
        this->servo_h_.write(this->angle_h_);
        this->servo_v_.write(this->angle_v_);
    }

    if ((this->elapsed_time_ > this->duration_) && this->is_swingwing_)
    {
        Serial.print(this->elapsed_time_);
        Serial.println(" : finish swinging");
        this->servo_h_.write(90);
        this->servo_v_.write(90);
        this->is_swingwing_ = false;
        this->elapsed_time_ = 0;
    }
}

void Tail::attachPin(int pin_h, int pin_v)
{
    initServo(this->servo_h_, pin_h);
    initServo(this->servo_v_, pin_v);
}

int average(int *values, int length)
{
    int sum = 0;
    for (int i = 0; i < length; i++)
    {
        sum += values[i];
    }
    return sum / length;
}

void pushBack(int *values, int length, int new_value)
{
    for (int i = length - 1; i > 0; i--)
    {
        values[i] = values[i - 1];
    }
    values[0] = new_value;
}

#endif