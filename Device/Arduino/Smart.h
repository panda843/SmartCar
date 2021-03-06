#include <Servo.h>
#include <Arduino.h>
#include "AFMotor.h"
#include "IRremote.h"

#ifndef _SmartCar_h_
#define _SmartCar_h_

//定义日志输出
#define IS_DEBUG false
//定义波特率
#define BAUD_RATE 38400
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
//定义红外线接收器(2,3:为中断信号针脚)
#define INFRARED_RECEIVER 2

class SmartCar{
    public:
        SmartCar(void);
        void initSmartCar(void);
        void cameraHTurn(int number);
        void cameraVTurn(int number);
        int getCameraHPos(void);
        int getCameraVPos(void);
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
        static void remoteControl(void);
        void autoRun(void);
        void setAutomatic(bool isEnable);
        void logs(const char* log);
    private:
        /**
         * servo1 相机水平舵机
         * servo2 相机上下舵机
         * servo3 超声波舵机
         */
        Servo servo1,servo2,servo3;
        /**
         * 定义自动OR手动
         * 默认自动模式
         */
        bool is_automatic = true;
        /**
         * 红外信号结构体
         */
        decode_results ir_result;
        void steeringTurn(Servo* servo, int number);
};

#endif
