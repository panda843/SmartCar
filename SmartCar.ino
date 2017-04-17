#include <Servo.h>
#include "SteeringEngine.h"

SteeringEngine servo;
     
void setup() { 
  servo.init();
} 
  
void loop() { 
    servo.moveOne(180);
    servo.moveOne(0);
    servo.moveTwo(180);
    servo.moveTwo(0);
}
