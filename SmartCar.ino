#include <Servo.h> 
#include "Smart.h"
#include "IRremote.h"

SmartCar car;

int RECV_PIN = 13;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup(){
      delay(2000);
       irrecv.enableIRIn(); // Start the receiver
      car.initSmartCar();
}

void yaokong(){
    
}

void loop(){
  car.cameraHTurn(20);
  car.ultrasonicVTrun(90);
  float center = car.detectionRange();
  if(center < 40){
    car.stopMotor();
//    car.ultrasonicVTrun(0);
//    float right = car.detectionRange();
//    car.ultrasonicVTrun(150);
//    float left = car.detectionRange();
    float right = car.detectionRangeRight();
    float left = car.detectionRangeLeft();
    car.ultrasonicVTrun(90);
    if(left <= right){
      car.goTurnRight();
      for(int i=0;i<70;i++){
        float trun_num = car.detectionRange();
        if(trun_num < 10){
          car.goBack();
        }else{
          car.goTurnRight();
        }
      }
      return void();
    }else{
      car.goTurnLeft();
      for(int i=0;i<70;i++){
        float trun_num = car.detectionRange();
        if(trun_num < 10){
          car.goBack();
        }else{
          car.goTurnLeft();
        }
      }
      return void();
    }
  }
  car.goForward();
//  if (irrecv.decode(&results)) {
//     Serial.println(results.value);
//      if(results.value == 16736925){
//              //Model
//              car.goForward();
//      }else if(results.value ==16712445 ){
//          //快退
//          car.stopMotor();
//      }else if(results.value ==16720605 ){
//          //播放暂停
//          car.goTurnRight();
//      }else if(results.value ==16761405){
//          //快进
//           car.goTurnLeft();
//      }else if(results.value ==16754775){
//          //音量减
//          car.goBack();
//      }else if(results.value ==16769055 ){
//          //EQ
//          car.cameraVTurn(180);
//          car.cameraVTurn(0);
//      }else if(results.value == 16748655){
//          //音量加
//          car.cameraHTurn(180);
//          car.cameraHTurn(0);
//      }
//      irrecv.resume(); // Receive the next value
//    }    
//  delay(2000);
//  //前进
//  car.goForward();
//  delay(2000);  
//  //后退
//  car.goBack();
//  delay(2000); 
//  //左转
//  car.goTurnLeft();
//  delay(2000);
//  //右转
//  car.goTurnRight();
//  delay(2000);
//  //停止
//  car.stopMotor();
//  delay(2000);
//  //相机水平转动
//  car.cameraVTurn(180);
//  car.cameraVTurn(0);
//  delay(2000);
//  //相机上下转动
//  car.cameraHTurn(180);
//  car.cameraHTurn(0);
}
