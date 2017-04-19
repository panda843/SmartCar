#include "Smart.h"

SmartCar car;

void setup(){
  delay(3000);
  car.initSmartCar();
}

void loop(){
  car.goForward();
}
