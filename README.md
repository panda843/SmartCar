# Raspberry Pi Zero W + Arduino Pro Mini 智能小车

##1.硬件
[x] Raspberry Pi Zero W (1)
[x] Arduino Pro Mini (1)
[x] L293D Motor Control Shield (1)
[x] HC-SR04 (1)
[x] KY-022 (1)
[x] SG90/TOWERPRO (3)
##2.代码
```c/c++
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
//定义红外线接收器(2,3:为中断信号针脚)
#define INFRARED_RECEIVER 2

class SmartCar{
    public:
	//构造函数
        SmartCar(void);
	//初始化函数,steup调用
        void initSmartCar(void);
	//相机上下转动
        void cameraHTurn(int number);
	//相机水平转动
        void cameraVTurn(int number);
	//超声波水平转动
        void ultrasonicVTrun(int number);
	//小车左转
        void goTurnLeft(void);
	//设置马达转动速度
        void setCarSpeed(int number);
	//超声波探测
        float detectionRange();
	//探测左边距离(40-90°)
        float detectionRangeLeft();
	//探测右边距离(90-130°)
        float detectionRangeRight();
	//小车右转
        void goTurnRight(void);
	//小车前进
        void goForward(void);
	//小车后退
        void goBack(void);
	//小车停止
        void stopMotor(void);
	//红外遥控
        void remoteControl(void);
	//超声波自动运行(自动避障)
        void autoRun(void);
	//设置手动OR自动
        void setAutomatic(bool isEnable);
	//打印日志
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
	//舵机转动
        void steeringTurn(Servo* servo, int number);
};
```
