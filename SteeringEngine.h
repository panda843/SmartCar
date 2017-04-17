#include <Servo.h>
#include <Arduino.h>
// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#ifndef _SteeringEngine_h_
#define _SteeringEngine_h_

class SteeringEngine{
    public:
        SteeringEngine(void);
        SteeringEngine* init(void);
        SteeringEngine* setDriveOne(int number);
        SteeringEngine* setDriveTwo(int number);
        SteeringEngine* setBaudRate(int number);
        SteeringEngine* moveOne(int pos);
        SteeringEngine* moveTwo(int pos);
    private:
        Servo servo_one,servo_two;
        int baud_rate = 9600;
        int drive_one = 9;
        int drive_two = 10;
        int pos_one = 0;
        int pos_two = 0;
};

#endif
