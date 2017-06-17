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
            Serial.write("equals:"+str);
        }else{
            Serial.write(str);
        }
        str = "";
    }
}
