#include <NewPing.h>
#include <Stepper.h>
#include <Time.h>

// Stepper motor defenitions and constructor 
#define STEPS 200
#define ONE_REV ((360 / 5.625) * 32)
#define ONE_CM (ONE_REV / 18.2)
#define WHEELS_DIST (16.7)

#define LEFT_FORWARD (1)
#define LEFT_BACK (-1)
#define RIGHT_FORWARD (-1)
#define RIGHT_BACK (1)

Stepper stepperLeft(STEPS, 8, 10, 9, 11);
Stepper stepperRight(STEPS, 4, 6, 5, 7);

// Ultrasonic defenitions and constructor 
#define SR_LEFT_TRIG A1
#define SR_LEFT_ECHO A0

#define SR_RIGHT_TRIG 2
#define SR_RIGHT_ECHO 3

#define SR_FRONT_TRIG 12
#define SR_FRONT_ECHO 13

#define MAX_PING_DIST 100

NewPing left_hcsr04(SR_LEFT_TRIG, SR_LEFT_ECHO, MAX_PING_DIST);
NewPing right_hcsr04(SR_RIGHT_TRIG, SR_RIGHT_ECHO, MAX_PING_DIST);
NewPing front_hcsr04(SR_FRONT_TRIG, SR_FRONT_ECHO, MAX_PING_DIST);

//Timer for SR04
#define SR_TIMER 500

//Global variables
unsigned long leftDist, rightDist, frontDist;
unsigned long curTime;

void setup(){
  //Serial initilization
  Serial.begin(9600);
  
  // set the speed of the motor to 30 RPMs
  stepperLeft.setSpeed(200);
  stepperRight.setSpeed(200);

  //set current time
  curTime = millis();
}

void readUltrasonicData(void) {
  leftDist = left_hcsr04.ping_cm();
  rightDist = right_hcsr04.ping_cm();
  frontDist = front_hcsr04.ping_cm();

  //     Output the distance in mm
//  Serial.print("left: " + String(leftDist));
//  Serial.print("\tright :" + String(rightDist));
//  Serial.println("\tfront :" + String(frontDist));
}

void loop() {
//  if(millis() - curTime > SR_TIMER) {
//    readUltrasonicData();
//    curTime = millis();
//  }
  readUltrasonicData();
  if(frontDist > 10) {
  stepperRight.step(RIGHT_FORWARD * 10);
  stepperLeft.step(LEFT_FORWARD * 10);
  }

}
