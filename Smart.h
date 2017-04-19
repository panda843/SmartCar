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
//定义马达速度
#define MOTOR_SPEED 200

class SmartCar{
    public:
        SmartCar(void);
        void initSmartCar(void);
        void cameraHTurn(int number);
        void cameraVTurn(int number);
        void goTurnLeft(void);
        void goTurnRight(void);
        void goForward(void);
        void goBack(void);
        void stopMotor(void);
        void logs(const char* log);
    private:
        Servo servo1,servo2;  
};

#endif
