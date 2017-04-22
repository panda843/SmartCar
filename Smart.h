#include <Servo.h>
#include <Arduino.h>
#include "AFMotor.h"

#ifndef _SmartCar_h_
#define _SmartCar_h_

//定义日志输出
#define IS_DEBUG true
//定义波特率
#define BAUD_RATE 9600
//定义舵机1串口
#define STEERING_ONE 9
//定义舵机2串口
#define STEERING_TWO 10
//定义舵机3串口(超声波)
#define STEERING_THREE A0
//定义马达速度
#define MOTOR_SPEED 150
//定义马达转弯时的速度
#define MOTOR_TURN_SPEED 100
//定义超声波Tring引脚
#define ULTRASONIC_TRIG A1   
//定义超声波Echo引脚
#define ULTRASONIC_ECHO A2

class SmartCar{
    public:
        SmartCar(void);
        void initSmartCar(void);
        void cameraHTurn(int number);
        void cameraVTurn(int number);
        void ultrasonicVTrun(int number);
        void goTurnLeft(void);
        void setCarSpeed(int number);
        float detectionRange();
        float detectionRangeLeft();
        float detectionRangeRight();
        void goTurnRight(void);
        void goForward(void);
        void goBack(void);
        void stopMotor(void);
        void logs(const char* log);
    private:
        Servo servo1,servo2,servo3;  
        int pos1=0,pos2=0,pos3=0;
        void steeringTurn(Servo* servo, int* pos, int number);
};

#endif
