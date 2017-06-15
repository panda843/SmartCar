#include "Smart.h"

/**
 * 红外线接收器
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
static IRrecv irrecv(INFRARED_RECEIVER);

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
    //定义红外线接收器
    irrecv.enableIRIn();
    //设置中断，红外遥控
    attachInterrupt(0,(void(*)())&SmartCar::remoteControl,CHANGE);
    this->logs("MSG: infrared ok");
}

/**
 * 设置是否启动自动模式
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-24
 * @param    isEnable                         true:自动,false:红外遥控
 */
void SmartCar::setAutomatic(bool isEnable){
    this->is_automatic = isEnable;
}

/**
 * 自动运行模式
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-24
 */
void SmartCar::autoRun(void){
    this->cameraHTurn(20);
    this->ultrasonicVTrun(90);
    float center = this->detectionRange();
    if(center < 40){
        this->stopMotor();
        float right = this->detectionRangeRight();
        float left = this->detectionRangeLeft();
        this->ultrasonicVTrun(90);
        if(left <= right){
            this->goTurnRight();
            for(int i=0;i<70;i++){
                float trun_num = this->detectionRange();
                if(trun_num < 10){
                    this->goBack();
                }else{
                    this->goTurnRight();
                }
            }
            return void();
        }else{
            this->goTurnLeft();
            for(int i=0;i<70;i++){
                float trun_num = this->detectionRange();
                if(trun_num < 10){
                    this->goBack();
                }else{
                    this->goTurnLeft();
                }
            }
            return void();
        }
    }
    this->goForward();
}

/**
 * 红外遥控
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-24
 */
void SmartCar::remoteControl(void){
    if(!this->is_automatic){
        if (irrecv.decode(&this->ir_result)) {
            Serial.println(this->ir_result.value);
            if(this->ir_result.value == 16736925){
                //Model
                this->goForward();
            }else if(this->ir_result.value == 16712445){
                //快退
                this->stopMotor();
            }else if(this->ir_result.value == 16720605){
                //播放暂停
                this->goTurnRight();
            }else if(this->ir_result.value == 16761405){
                //快进
                this->goTurnLeft();
            }else if(this->ir_result.value == 16754775){
                //音量减
                this->goBack();
            }else if(this->ir_result.value == 16769055){
                //EQ
                int number = this->servo2.read();
                if(number < 180){
                    this->cameraVTurn(number+10);
                }else{
                    this->cameraVTurn(number-10);
                }
            }else if(this->ir_result.value == 16748655){
                //音量加
                int number = this->servo1.read();
                if(number < 180){
                    this->cameraHTurn(number+10);
                }else{
                    this->cameraHTurn(number-10);
                }
            }
            //接受下一个值
            irrecv.resume(); 
        }
    }
}
/**
 * 小车前进
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
void SmartCar::goForward(void){
    this->logs("MSG: motor ↑");
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    motor3.run(BACKWARD);
    motor4.run(BACKWARD);
}

/**
 * 小车后退
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 */
void SmartCar::goBack(void){
    this->logs("MSG: motor ↓");
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    motor3.run(FORWARD);
    motor4.run(FORWARD);
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

 void SmartCar::setCarSpeed(int number){
    motor1.setSpeed(number);
    motor2.setSpeed(number);
    motor3.setSpeed(number);
    motor4.setSpeed(number);
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
    this->steeringTurn(&this->servo1,number);
}

/**
 * 相机上下转动
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 * @param    number                           转动角度 0-180
 */
void SmartCar::cameraVTurn(int number){
    this->logs("MSG: servo Vertical move ");
    this->steeringTurn(&this->servo2,number);
}

/**
 * 超声波舵机水平转动
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 * @param    number                           转动角度 0-180
 */
void SmartCar::ultrasonicVTrun(int number){
    this->logs("MSG: ultrasonic Vertical move ");
    this->steeringTurn(&this->servo3,number);
}

/**
 * 舵机转动控制
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-21
 * @param    servo                            Servo对象
 * @param    number                           转动角度 0-180
 */
void SmartCar::steeringTurn(Servo* servo, int number){
    //获取当前舵机角度
    int current_pos = servo->read();
    if(current_pos != number){
        if(current_pos < number){
            for(current_pos; current_pos < number; current_pos += 1) {
                servo->write(current_pos);
                delay(15);
            }
        }else{
            for(current_pos; current_pos>=number; current_pos-=1){                                
                servo->write(current_pos);
                delay(15);
            } 
        }
    }    
}

/**
 * 超声波探测左边的距离(40-90°)
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-24
 * @return   返回最远的距离
 */
float SmartCar::detectionRangeLeft(){
  this->ultrasonicVTrun(90);
  int current_pos = 90;
  float max_num = 0.0;
  for(int i=90;i>=40;i-=10){
    this->ultrasonicVTrun(i);
    float current_number = this->detectionRange();
    if(current_number > max_num){
      max_num = current_number;
    }
  }
  return max_num;
}

/**
 * 超声波探测右边的距离(90-130°)
 * @Author   DuanEnJian<backtrack843@163.com>
 * @DateTime 2017-04-24
 * @return   返回最远的距离
 */
float SmartCar::detectionRangeRight(){
  this->ultrasonicVTrun(90);
  int current_pos = 90;
  float max_number = 0.0;
  for(int i=90;i<=130;i+=10){
    this->ultrasonicVTrun(i);
    float current_number = this->detectionRange();
    if(current_number > max_number){
      max_number = current_number;
    }
  }
  return max_number;
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
