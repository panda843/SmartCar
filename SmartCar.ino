#include <Servo.h>
#include "Smart.h"

SmartCar car;
     
void setup() { 
  car.initSmartCar();
} 
  
void loop() { 
    car.goForward();
}
