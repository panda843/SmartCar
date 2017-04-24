#include <Servo.h> 
#include "Smart.h"

SmartCar car;

void setup(){
    delay(2000);
    car.initSmartCar();
}
void loop(){
    car.autoRun();
}
