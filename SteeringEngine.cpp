#include "SteeringEngine.h"


SteeringEngine::SteeringEngine(void){
  
}

SteeringEngine* SteeringEngine::init(void){
    Serial.begin(this->baud_rate);
    this->servo_one.attach(this->drive_one);
    this->servo_two.attach(this->drive_two);
    return this;
}

SteeringEngine* SteeringEngine::setBaudRate(int number){
    this->baud_rate = number;
    Serial.begin(this->baud_rate);
    return this;
}

SteeringEngine* SteeringEngine::setDriveOne(int number){
    this->drive_one = number;
    this->servo_one.attach(this->drive_one);
    return this;
}

SteeringEngine* SteeringEngine::setDriveTwo(int number){
    this->drive_two = number;
    this->servo_two.attach(this->drive_two);
    return this;
}
SteeringEngine* SteeringEngine::moveOne(int pos){
    pos = (pos < 0) ? 0 : (pos > 180) ? 180 : pos;
    this->pos_one = this->servo_one.read();
    if(this->pos_one < pos){
        for(this->pos_one; this->pos_one < pos; this->pos_one++){
            this->servo_one.write(this->pos_one);          
            delay(10);                       
        }
    }else{
        for(this->pos_one; this->pos_one > pos; this->pos_one--){
            this->servo_one.write(this->pos_one);          
            delay(10);                      
        }
    }
}
SteeringEngine* SteeringEngine::moveTwo(int pos){
    pos = (pos < 0) ? 0 : (pos > 180) ? 180 : pos;
    this->pos_two = this->servo_two.read();
    if(this->pos_two < pos){
        for(this->pos_two; this->pos_two < pos; this->pos_two++){
            this->servo_two.write(this->pos_two);          
            delay(10);                       
        }
    }else{
        for(this->pos_two; this->pos_two > pos; this->pos_two--){
            this->servo_two.write(this->pos_two);          
            delay(10);                      
        }
    }
}
