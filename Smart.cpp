#include "Smart.h"

static AF_DCMotor motor1(1, MOTOR12_1KHZ);
static AF_DCMotor motor2(2, MOTOR12_1KHZ); 
static AF_DCMotor motor3(3, MOTOR34_1KHZ); 
static AF_DCMotor motor4(4, MOTOR34_1KHZ);


SmartCar::SmartCar(void){
  
}

void SmartCar::logs(const char* log){
    if(IS_DEBUG){
        Serial.println(log);
    }
}

/**
 * 初始化小车 setup调用
 */
void SmartCar::initSmartCar(void){
    //设置通信串口
    Serial.begin(BAUD_RATE);
    this->logs("MSG: begin ok");
    Serial.println();
    //设置舵机串口
    this->servo1.attach(STEERING_ONE);
    this->servo2.attach(STEERING_TWO);
    this->logs("MSG: servo ok");
    //设置马达转速
    motor1.setSpeed(MOTOR_SPEED);
    motor2.setSpeed(MOTOR_SPEED);
    motor3.setSpeed(MOTOR_SPEED);
    motor4.setSpeed(MOTOR_SPEED);
    this->logs("MSG: motor ok");
}

/**
 * 前进
 */
void SmartCar::goForward(void){
    this->logs("MSG: motor ↑");
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
}

/**
 * 后退
 */
void SmartCar::goBack(void){
    this->logs("MSG: motor ↓");
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
}

/**
 * 停止
 */
void SmartCar::stopMotor(void){
    this->logs("MSG: motor stop");
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    motor3.run(RELEASE);
    motor4.run(RELEASE);
}

/**
 * 左转
 */
void SmartCar::goTurnLeft(void){
    this->logs("MSG: motor ←");
    motor1.run(BACKWARD);
    motor3.run(BACKWARD);
    motor2.run(FORWARD);
    motor4.run(FORWARD);
}

/**
 * 右转
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
 * @params int number 转动角度 0-180
 */
void SmartCar::cameraHTurn(int number){
    this->logs("MSG: servo horizontal move ");
    int current_pos = servo1.read();
    if(current_pos < number){
        for(this->pos1 = current_pos; this->pos1 < number; this->pos1 += 1) {
          servo1.write(this->pos1);
          delay(15);
        }
    }else{
        for(this->pos1 = current_pos; this->pos1>=number; this->pos1-=1){                                
          servo1.write(this->pos1);
          delay(15);
        } 
    }
}

/**
 * 相机上下转动
 */
void SmartCar::cameraVTurn(int number){
    this->logs("MSG: servo Vertical move ");
    int current_pos = servo2.read();
    if(current_pos < number){
        for(this->pos2 = current_pos; this->pos2 < number; this->pos2 += 1) {
            servo2.write(this->pos2);
            delay(15);
        }
    }else{
        for(this->pos2 = current_pos; this->pos2>=number; this->pos2-=1){                                
            servo2.write(this->pos2);
            delay(15);
        } 
    }
}
