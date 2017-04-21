#include <Servo.h> 
#include "Smart.h"

SmartCar car;

void setup(){
      delay(2000);
      car.initSmartCar();
}

void loop(){
  delay(2000);
  //前进
  car.goForward();
  delay(2000);  
  //后退
  car.goBack();
  delay(2000); 
  //左转
  car.goTurnLeft();
  delay(2000);
  //右转
  car.goTurnRight();
  delay(2000);
  //停止
  car.stopMotor();
  delay(2000);
  //相机水平转动
  car.cameraVTurn(180);
  car.cameraVTurn(0);
  delay(2000);
  //相机上下转动
  car.cameraHTurn(180);
  car.cameraHTurn(0);
}
