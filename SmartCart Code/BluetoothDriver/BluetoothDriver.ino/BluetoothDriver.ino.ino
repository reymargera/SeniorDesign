#include "BMSerial.h"
#include "RoboClaw.h"
#include <SonarSensor.h>


//Roboclaw Address
#define address 0x80

//Speeds
#define FULLSPEED 25
#define HALFSPEED 10
#define TURNSPEEDFORWARD 60
#define TURNSPEEDBACKWARD 35

//Setup communcaitions with roboclaw. Use pins 10 and 11 with 10ms timeout
RoboClaw roboclaw(10,11,10000);


SonarSensor sonar;
String inputString = "";
int val=0;
         // variable to receive data from the serial port

boolean stringComplete = false;
void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(8, INPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);

  roboclaw.begin(38400);

  inputString.reserve(50);
  }

  
void loop() {
  if (inputString == "on\n")
  {
   val = 1;
  Serial.println("on was printed"); 
  }
  else if (inputString == "off\n")
 { val = 0;
 Serial.println("off was printed");
}
else 
{Serial.println("data is beng sent");
}
  if (val==1)
  {
    drive();
  }
  else 
  {
    stopMoving();
  }
}
void serialEvent() {
  inputString = "";
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    //Serial.write(inChar);
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline or a carriage return, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n' || inChar == '\r') {
      stringComplete = true;
    } 
  }
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
      moveStraightFull();
    } 
    
 }

void moveStraightFull() {
   Serial.println("Full Speed");
   roboclaw.ForwardM1(address,25);
   roboclaw.ForwardM2(address,25);
}

void moveStraightHalf() {
  Serial.println("Half Speed");
  roboclaw.ForwardM1(address,8);
  roboclaw.ForwardM2(address,8);
}

void stopMoving() {
  Serial.println("Stopped");
  roboclaw.ForwardM1(address,0);
  roboclaw.ForwardM2(address,0);
}

void turnRight() {
  int i;
  unsigned long start = millis();
  while (millis() - start <= 1000) {
    roboclaw.ForwardM1(address,TURNSPEEDFORWARD);
    roboclaw.BackwardM2(address,TURNSPEEDBACKWARD);
  }
  start = millis();
  while (millis() - start <= 1000) {
    moveStraightHalf();
  }
  
  start = millis();
  while (millis() - start <= 1000) {
    roboclaw.ForwardM2(address,TURNSPEEDFORWARD);
    roboclaw.BackwardM1(address,TURNSPEEDBACKWARD);
  }
}

void turnLeft() {
  int i;
  unsigned long start = millis();
  while (millis() - start <= 1000) {
    roboclaw.BackwardM1(address,TURNSPEEDBACKWARD);
    roboclaw.ForwardM2(address,TURNSPEEDFORWARD);
  }
  start = millis();
   while (millis() - start <= 1000) {
    moveStraightHalf();
  }
  start = millis();
   while (millis() - start <= 1000) {
    roboclaw.BackwardM2(address,TURNSPEEDBACKWARD);
    roboclaw.ForwardM1(address,TURNSPEEDFORWARD);
  }
}

bool aboveThresholdTemp(int threshold) {
  int s1 = sonar.readSensor1();
  int s2 = sonar.readSensor2();
  int s3 = sonar.readSensor3();
  
  if(s1 < threshold) {
    Serial.print("Sonar 1 Fail");
    Serial.print(s1);
    return false;
  }
  else if(s2 < threshold) {
    Serial.print("Sonar 2 Fail");
    Serial.print(s2);
    return false;
  }
  else if(s3 < threshold) {
    Serial.print("Sonar 3 Fail");
    Serial.print(s3);
    return false;
  }
    return true;
}

int decideTurn() {
  int leftPath = sonar.readSensor1();
  int rightPath = sonar.readSensor2();
  if(leftPath > rightPath)
    return 1;
  else
    return 2;
}





