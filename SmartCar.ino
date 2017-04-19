#include "Smart.h"
#include "IRremote.h"

SmartCar car;

int RECV_PIN = 13; // 红外一体化接收头连接到Arduino 11号引脚
 
IRrecv irrecv(RECV_PIN);
 
decode_results results; // 用于存储编码结果的对象

void setup(){
  delay(3000);
  car.initSmartCar();

   irrecv.enableIRIn(); //初始化红外遥控
}

void loop(){
   if (irrecv.decode(&results)) {
        Serial.println(results.value);
        irrecv.resume(); // 接收下一个编码
   }
}
