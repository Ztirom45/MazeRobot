/*
a siple test program for my robot Setup written with c++ and PlatformIo
there are No Competible Motor Driver Pins or general PWM pins to power the Motors of my Robot
=> The RObot Setup Doesn't Work

LICENCE: GPL4
written by Ztirom45
*/

#include <Arduino.h>
#include <MeMCore.h>
#include <Wire.h>
//sensors:
MeGyro gyro;

//motor pins [L]eft/[R]ight [F]orward/[B]ackward
#define pinMotorLF 9
#define pinMotorLB 10
#define pinMotorRF 5
#define pinMotorRB 6

#define move(left,right){\
  analogWrite((left>0) ? pinMotorLF : pinMotorLB, left);\
  analogWrite((right>0) ? pinMotorRF : pinMotorRB, right);\
}
#define stop(){\
  digitalWrite(pinMotorLF, LOW);\
  digitalWrite(pinMotorLB, LOW);\
  digitalWrite(pinMotorRF, LOW);\
  digitalWrite(pinMotorLB, LOW);\
}


/*
class for advanced robot movement
  like move for time, or gyro
  
  make sure, to reset move_finish via prepare_move bofore a moving
*/
class Chassis{
  public:
    bool is_moving = false;
    bool move_finish = false;
    int last_angle = 0;
    int last_time = 0;
    MeGyro *ptrGyro;

    Chassis(MeGyro *ptrGyroInp): ptrGyro(ptrGyroInp){
    };

    inline void begin(){  
      pinMode(LED_BUILTIN, pinMotorLF);
      pinMode(LED_BUILTIN, pinMotorLB);
      pinMode(LED_BUILTIN, pinMotorRF);
      pinMode(LED_BUILTIN, pinMotorRB);
    };

    void move_angle(int speed,int angle){
      if(this->move_finish){return;}
      
      if(angle==0){return;}
      
      gyro.update();
      Serial.read();
      if(!this->is_moving){
        this->last_angle = this->ptrGyro->getAngleZ();
        if(angle>0){//right
          move(speed,-speed);
        }else{//left
          move(-speed,speed);
        }
        this->is_moving = true;
      }
      
      if(angle>0){//right
        int angle_now = this->ptrGyro->getAngleZ()- ((this->last_angle-angle<-180)? 360 : 0);
        if(this->last_angle-angle > angle_now){
          stop();
          this->move_finish = true;
          this->is_moving = false;
        }
        return;
        
      }
      //else left
      int angle_now = this->ptrGyro->getAngleZ()+ ((this->last_angle+angle>180)? 360 : 0);
      
      if(this->last_angle-angle < angle_now){
        stop();
        this->move_finish = true;
        this->is_moving = false;
      }
      return;

    };
    void move_time(int left,int right,int ms){
      if(this->move_finish || ms<0){return;}
       
      if(!this->is_moving){
        this->last_time = millis();
        is_moving = true;
        move(left,right);
      }
      
      if(ms<millis()-last_time){
        stop();
        this->is_moving = false;
        this->move_finish = true;
      }
      
      
    };

    void prepare_move(){
      this->is_moving = false;
      this->move_finish = false;
    }
      
      

};

Chassis chassis(&gyro);

void setup() {

  //setup printf
  Serial.begin(9600);
  Serial.print("hi2\n");
  pinMode(LED_BUILTIN, OUTPUT);
  //init sensors
  gyro.begin();
  chassis.begin();

  chassis.prepare_move();
  chassis.move_angle(100,90);
  while(!chassis.move_finish){
    Serial.print("X: ");
    Serial.print(gyro.getAngleX());
    Serial.print(" Y: ");
    Serial.print(gyro.getAngleY());
    Serial.print(" Z : ");
    Serial.println(gyro.getAngleZ());
    chassis.move_angle(100,90);
  }
  chassis.prepare_move();
  chassis.move_time(100,100,2000);
  while(!chassis.move_finish){
    Serial.println(millis());
    chassis.move_time(100,100,2000);
  }
}

// the loop function runs over and over again forever
void loop() {

}
