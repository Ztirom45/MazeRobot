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
//ultrasonic ME 3
//ultrasonic 12,13

//motor pins [L]eft/[R]ight [F]orward/[B]ackward
#define pinMotorLF 9
#define pinMotorLB 10
#define pinMotorRF 5
#define pinMotorRB 6

#define move(left,right){\
  analogWrite((left>0) ? pinMotorLF : pinMotorLB, abs(left));\
  analogWrite((right>0) ? pinMotorRF : pinMotorRB, abs(right));\
}
#define stop(){\
  digitalWrite(pinMotorLF, LOW);\
  digitalWrite(pinMotorLB, LOW);\
  digitalWrite(pinMotorRF, LOW);\
  digitalWrite(pinMotorRB, LOW);\
}


/*
class for advanced robot movement
  like move for time, or gyro
  
  make sure, to reset move_finish via prepare_move bofore a moving
*/
void move_angle(int angle,int speed,MeGyro* ptrGyro){
  if(angle>0){//right
    move(speed,-speed);
  }else{//left
    move(-speed,speed);
  }

  Serial.read();
  gyro.update();
  int last_angle = gyro.getAngleZ();
  
  if(angle>0){//right
    int angle_now = ptrGyro->getAngleZ() - ((last_angle-angle<-180)? 360 : 0);
    while(last_angle-angle < angle_now){
      Serial.read();
      gyro.update();
      angle_now = ptrGyro->getAngleZ() - ((last_angle-angle<-180)? 360 : 0);
      Serial.print("X: ");
      Serial.println(angle_now);
      //Serial.println(gyro.getAngleZ());
    }
    stop();
    return;
  }
  
  //else left
  int angle_now = ptrGyro->getAngleZ()+ ((last_angle+angle>180)? 360 : 0);
  while(last_angle-angle > angle_now){  
    Serial.read();
    gyro.update();
    angle_now = ptrGyro->getAngleZ()+ ((last_angle+angle>180)? 360 : 0);
    Serial.print("X: ");
    Serial.println(angle_now);

    
  }
  stop();
}

double ultrasonic_cm(int trig_pin,int echo_pin,double conversion_factor){
  //trigger
  pinMode(trig_pin, OUTPUT);
  digitalWrite(trig_pin,LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin,LOW);

  //read
  pinMode(echo_pin,INPUT);
  return (double)pulseIn(echo_pin,HIGH) *conversion_factor;
}

#define read_me_ultrasonic() ultrasonic_cm(3,3,1.0/58.0)
#define read_ultrasonic() ultrasonic_cm(13,12,0.034 / 2.0)

void setup() {
  //setup pins
  pinMode(pinMotorRF,OUTPUT);
  pinMode(pinMotorRF,OUTPUT);
  pinMode(pinMotorRB,OUTPUT);
  pinMode(pinMotorLF,OUTPUT);
  pinMode(pinMotorLB,OUTPUT);

  //setup printf
  Serial.begin(9600);
  Serial.print("hi2\n");
  pinMode(LED_BUILTIN, OUTPUT);
  //init sensors and motors
  gyro.begin();
  stop();
 
}

// the loop function runs over and over again forever
#define TURN_INTENCITY 1

void loop() {

  //wall follower
  int wall_distance = (int)read_ultrasonic();
  Serial.println(wall_distance);
  move((wall_distance<10)?  100+(10-wall_distance)*TURN_INTENCITY  : 75,
       (wall_distance>10)?  100+(wall_distance-10)*TURN_INTENCITY  : 75);

  
  delay(23);//better sensor values
  //cheak for walls in front
  if(read_me_ultrasonic()<15){
    stop();
    delay(500);
    move_angle(90,150,&gyro);
    stop();
    gyro.begin();//reset
    delay(500);

  }
  delay(30);//better sensor values

}
