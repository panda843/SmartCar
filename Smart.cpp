#include "Smart.h"

/**
 * 直流电机1
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
static AF_DCMotor motor1(1, MOTOR12_1KHZ);

/**
 * 直流电机2
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
static AF_DCMotor motor2(2, MOTOR12_1KHZ); 

/**
 * 直流电机3
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
static AF_DCMotor motor3(3, MOTOR34_1KHZ); 

/**
 * 直流电机4
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
static AF_DCMotor motor4(4, MOTOR34_1KHZ);

/**
 * 构造函数
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
SmartCar::SmartCar(void){
  
}

/**
 * 日志输出
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 * @param    log                              日志
 */
void SmartCar::logs(const char* log){
    if(IS_DEBUG){
        Serial.println(log);
    }
}

/**
 * 初始化小车 setup调用
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
void SmartCar::initSmartCar(void){
    //设置通信串口
    Serial.begin(BAUD_RATE);
    this->logs("MSG: begin ok");
    Serial.println();
    //设置舵机串口
    this->servo1.attach(STEERING_ONE);
    this->servo2.attach(STEERING_TWO);
    this->servo3.attach(STEERING_THREE);
    this->logs("MSG: servo ok");
    //设置马达转速
    motor1.setSpeed(MOTOR_SPEED);
    motor2.setSpeed(MOTOR_SPEED);
    motor3.setSpeed(MOTOR_SPEED);
    motor4.setSpeed(MOTOR_SPEED);
    this->logs("MSG: motor ok");
    //定义超声波接口
    pinMode(ULTRASONIC_TRIG, OUTPUT);  
    pinMode(ULTRASONIC_ECHO, INPUT);
    this->logs("MSG: ultrasonic ok");
}

/**
 * 小车前进
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
void SmartCar::goForward(void){
    this->logs("MSG: motor ↑");
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
}

/**
 * 小车后退
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
void SmartCar::goBack(void){
    this->logs("MSG: motor ↓");
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
}

/**
 * 小车停止
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
void SmartCar::stopMotor(void){
    this->logs("MSG: motor stop");
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
}

/**
 * 小车左转
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
void SmartCar::goTurnLeft(void){
    this->logs("MSG: motor ←");
    motor1.run(BACKWARD);
    motor3.run(BACKWARD);
    motor2.run(FORWARD);
    motor4.run(FORWARD);
}

/**
 * 小车右转
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
void SmartCar::goTurnRight(void){
    this->logs("MSG: motor →");
    motor2.run(BACKWARD);
    motor4.run(BACKWARD);
    motor1.run(FORWARD);
    motor3.run(FORWARD);
}

/**
 * 相机水平转动
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 * @param    number                           转动角度 0-180
 */
void SmartCar::cameraHTurn(int number){
    this->logs("MSG: servo horizontal move ");
    this->steeringTurn(&this->servo1,&this->pos1,number);
//    int current_pos = servo1.read();
//    if(current_pos < number){
//        for(this->pos1 = current_pos; this->pos1 < number; this->pos1 += 1) {
//          servo1.write(this->pos1);
//          delay(15);
//        }
//    }else{
//        for(this->pos1 = current_pos; this->pos1>=number; this->pos1-=1){                                
//          servo1.write(this->pos1);
//          delay(15);
//        } 
//    }
}

/**
 * 相机上下转动
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 * @param    number                           转动角度 0-180
 */
void SmartCar::cameraVTurn(int number){
    this->logs("MSG: servo Vertical move ");
    this->steeringTurn(&this->servo2,&this->pos2,number);
//    int current_pos = servo2.read();
//    if(current_pos < number){
//        for(this->pos2 = current_pos; this->pos2 < number; this->pos2 += 1) {
//            servo2.write(this->pos2);
//            delay(15);
//        }
//    }else{
//        for(this->pos2 = current_pos; this->pos2>=number; this->pos2-=1){                                
//            servo2.write(this->pos2);
//            delay(15);
//        } 
//    }
}

/**
 * 超声波舵机水平转动
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 * @param    number                           转动角度 0-180
 */
void SmartCar::ultrasonicVTrun(int number){
    this->logs("MSG: ultrasonic Vertical move ");
    this->steeringTurn(&this->servo3,&this->pos3,number);
}

/**
 * 舵机转动控制
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 * @param    servo                            Servo对象
 * @param    pos                              舵机当前角度 0-180
 * @param    number                           转动角度 0-180
 */
void SmartCar::steeringTurn(Servo* servo, int* pos, int number){
    int current_pos = servo->read();
    if(current_pos < number){
        for(*pos = current_pos; *pos < number; *pos += 1) {
            servo->write(*pos);
            delay(15);
        }
    }else{
        for(*pos = current_pos; *pos>=number; *pos-=1){                                
            servo->write(*pos);
            delay(15);
        } 
    }
}

/**
 * 超声波测距
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 * @return   float 返回距离(单位cm)
 */
float SmartCar::detectionRange(){
    float temp,cm;
    //给Trig发送一个低高低的短时间脉冲,触发测距 
    //给Trig发送一个低电平   
    digitalWrite(ULTRASONIC_TRIG, LOW);
    //等待 2微妙 
    delayMicroseconds(2);
    //给Trig发送一个高电平      
    digitalWrite(ULTRASONIC_TRIG,HIGH);
    //等待 10微妙   
    delayMicroseconds(10);
    //给Trig发送一个低电平      
    digitalWrite(ULTRASONIC_TRIG, LOW);   
    //存储回波等待时间 pulseIn函数会等待引脚变为HIGH,开始计算时间,再等待变为LOW并停止计时并返回脉冲的长度
    temp = float(pulseIn(ULTRASONIC_ECHO, HIGH));

    /**
     * 声速是:340m/1s 换算成 34000cm / 1000000μs => 34 / 1000
     * 因为发送到接收,实际是相同距离走了2回,所以要除以2
     * 距离(厘米)  =  (回波时间 * (34 / 1000)) / 2
     * 简化后的计算公式为 (回波时间 * 17)/ 1000
     */
    
    //把回波时间换算成cm
    cm = (temp * 17 )/1000;   
    return cm;
}
