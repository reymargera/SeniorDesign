#include "BMSerial.h"
#include "RoboClaw.h"
#include <SonarSensor.h>


//Roboclaw Address
#define address 0x80

//Speeds
#define FULLSPEED 25
#define HALFSPEED 10
#define TURNSPEEDFORWARD 40
#define TURNSPEEDBACKWARD 10

//Setup communcaitions with roboclaw. Use pins 10 and 11 with 10ms timeout
RoboClaw roboclaw(10,11,10000);

//Initiate Sonar Sensors
SonarSensor sonar;

void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(8, INPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);

  roboclaw.begin(38400);
 
}

void loop() {
  roboclaw.ForwardM1(address,64);
  roboclaw.ForwardM2(address,64);  
}

 void drive() {
    //If all sensors are clear for the next 1.5m (5ft)
    //Clear path ahead, can progress
    
    
    
    if(aboveThresholdTemp(150)) {
      moveStraightFull();
    }
    //If all sensors are clear for the next [0.6, 1.5]m (2-5 ft)
    //Obstruction in the nearby proximity, should slow down to prepare for halt/turn
    else if(aboveThresholdTemp(60)) {
      moveStraightHalf();
    }
    //If all sensors are clear for the next [0.3, 0.6]m (1.5-2 ft)
    //Obstruction in very close range, halt and figure out next move
    else if(aboveThresholdTemp(30)) {
      stopMoving();
      if(decideTurn() == 1)
        turnLeft();
      else
        turnRight();
    }
    else {
      stopMoving();
    } 
    
 }

void moveStraightFull() {
   Serial.println("Full Speed");
   roboclaw.ForwardM1(address,25);
   roboclaw.ForwardM2(address,25);
}

void moveStraightHalf() {
  Serial.println("Half Speed");
  roboclaw.ForwardM1(address,10);
  roboclaw.ForwardM2(address,10);
}

void stopMoving() {
  Serial.println("Stopped");
  roboclaw.ForwardM1(address,0);
  roboclaw.ForwardM2(address,0);
}

void turnRight() {
  int i;
  for(i = 0; i < 10; i++) {
    roboclaw.ForwardM1(address,TURNSPEEDFORWARD);
    roboclaw.BackwardM2(address,TURNSPEEDBACKWARD);
  }
}

void turnLeft() {
  int i;
  for(i = 0; i < 10; i++) {
    roboclaw.BackwardM1(address,TURNSPEEDBACKWARD);
    roboclaw.ForwardM2(address,TURNSPEEDFORWARD);
  }
}

bool aboveThresholdTemp(int threshold) {
  if(sonar.readSensor1() > threshold && sonar.readSensor2() > threshold)
    return true;
  return false;
}

int decideTurn() {
  int leftPath = sonar.readSensor1();
  int rightPath = sonar.readSensor2();
  if(leftPath > rightPath)
    return 1;
  else
    return 2;
}
