#include <Servo.h> 
#include "Smart.h"

SmartCar car;

String str; 

void setup(){
    delay(2000);
    car.initSmartCar();
}
void loop(){
    while(Serial.available()>0){
        str += char(Serial.read());
        delay(2);
    }
    if(str.length() > 0){
        if(str.equals("97")){
            //小车左
            car.goTurnLeft();
        }else if(str.equals("115")){
            //小车下
            car.goBack();
        }else if(str.equals("100")){
            //小车右
            car.goTurnRight();
        }else if(str.equals("119")){
            //小车上
            car.goForward();
        }else if(str.equals("105")){
            //相机上
            car.cameraHTurn(0);
            car.cameraHTurn(1);
        }else if(str.equals("106")){
            //相机左
            car.cameraVTurn(160);
            car.cameraVTurn(159);
        }else if(str.equals("107")){
            //相机下
            car.cameraHTurn(160);
            car.cameraHTurn(159);
        }else if(str.equals("108")){
            //相机右
            car.cameraVTurn(5);
            car.cameraVTurn(4);
        }else if(str.equals("32")){
            //小车停止
            car.stopMotor();
        }
        str = "";
    }
}
