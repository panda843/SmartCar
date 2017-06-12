#include <Servo.h> 
#include "Smart.h"

SmartCar car;

void setup(){
    delay(2000);
    car.initSmartCar();
}
void loop(){
    while(Serial.available()>0){
        delay(100);
        String data = Serial.readString();
        Serial.println(data, DEC);
    }
}
