#include <hcsr04.h>
#include <NewPing.h>
#include <Stepper.h>
#include "TimerOne.h"

// Stepper motor defenitions and constructor 
#define STEPS (200)
#define ONE_REV ((360 / 5.625) * 32)
#define ONE_CM (ONE_REV / 18.2)
#define WHEELS_DIST (16.7)
#define STEP_NUM (100)
#define LEFT_FORWARD (1) 
#define LEFT_BACK (-1)
#define RIGHT_FORWARD (-1)
#define RIGHT_BACK (1)

Stepper stepperLeft(STEPS, 8, 10, 9, 11);
Stepper stepperRight(STEPS, 4, 6, 5, 7);

// Ultrasonic definitions and constructor 
#define SR_LEFT_TRIG A1
#define SR_LEFT_ECHO A0

#define SR_RIGHT_TRIG 2
#define SR_RIGHT_ECHO 3

#define SR_FRONT_TRIG 12
#define SR_FRONT_ECHO 13

#define MAX_PING_DIST (350)
#define MIN_PING_DIST (2)
#define DEADEND_FWD_DIST (6)
#define DEADEND_SIDE_DIST (16)
#define BEFORE_TURN (23)
#define AFTER_TURN (15)


enum {
  forward, turn180, turnLeft, turnRight, pause, beforeTurnLeft
}state;

NewPing left_hcsr04(SR_LEFT_TRIG, SR_LEFT_ECHO, MAX_PING_DIST);
NewPing right_hcsr04(SR_RIGHT_TRIG, SR_RIGHT_ECHO, MAX_PING_DIST);
NewPing front_hcsr04(SR_FRONT_TRIG, SR_FRONT_ECHO, MAX_PING_DIST);
//HCSR04 left_hcsr04(SR_LEFT_TRIG, SR_LEFT_ECHO, MIN_PING_DIST, MAX_PING_DIST);
//HCSR04 right_hcsr04(SR_RIGHT_TRIG, SR_RIGHT_ECHO, MIN_PING_DIST, MAX_PING_DIST);
//HCSR04 front_hcsr04(SR_FRONT_TRIG, SR_FRONT_ECHO, MIN_PING_DIST, MAX_PING_DIST);

//Timer for SR04
#define SR_TIMER (500)

//Global variables
unsigned long leftDist, rightDist, frontDist;
bool leftWall, rightWall, frontWall;

int turnSteps;

void setup(){
  //Serial initilization
  Serial.begin(9600);
  
  // set the speed of the motor to 30 RPMs
  stepperLeft.setSpeed(200);
  stepperRight.setSpeed(200);
  leftDist = rightDist = frontDist = MAX_PING_DIST;
  leftWall = rightWall = frontWall = false;
  turnSteps  = 0;
  state = forward;
}

void readUltrasonicData(void) {
  leftDist = left_hcsr04.ping_cm();
  rightDist = right_hcsr04.ping_cm();
  frontDist = front_hcsr04.ping_cm();
  Serial.print("left: " + String(leftDist));
  Serial.print("\tright :" + String(rightDist));
  Serial.println("\tfront :" + String(frontDist));
}

void checkWalls()
{
  //checking walls
  if (rightDist < DEADEND_SIDE_DIST && rightDist != 0)
    rightWall = true;
  else
    rightWall = false;

  if (leftDist < DEADEND_SIDE_DIST && leftDist != 0)
    leftWall = true;
  else
    leftWall = false;

  if (frontDist < DEADEND_FWD_DIST && frontDist != 0)
    frontWall = true;
  else
    frontWall = false;
//
//  Serial.print("leftWall: " + String(leftWall));
//  Serial.print("\trightWall: " + String(rightWall));
//  Serial.println("\tfrontWall: " + String(frontWall));
}

void checkState()
{
  if (state == turnRight || state == turnLeft || state == turn180 || state == beforeTurnLeft)
    return;
  state = forward;
  if (!leftWall)
    state = beforeTurnLeft;
  else if (!frontWall)
    state = forward;
  else if (!rightWall)
    state = turnRight;
  else
    state = turn180;
}

void moveSteps(int rightMove, int leftMove, int steps)
{
    for (int i = 0; i < steps; i++)
    {
      stepperLeft.step(LEFT_FORWARD * leftMove);
      stepperRight.step(RIGHT_FORWARD * rightMove);
    }
}

void moveByState()
{
  if (state == pause)
      delay(1000);
  else if (state == forward)
      moveSteps(1, 1, STEP_NUM);
  else if (state == beforeTurnLeft)
  {
    moveSteps(1, 1, (BEFORE_TURN) * ONE_CM);
    state = turnLeft;
  }
  else if (state == turnLeft)
  {
    moveSteps(1, -1, ((WHEELS_DIST - 1) * PI / 4) * ONE_CM);
    state = forward;
  }
  else if (state == turnRight)
  {
    moveSteps(-1, 1, ((WHEELS_DIST + 0.35) * PI / 4) * ONE_CM);
    state = forward;
  }
  else if (state == turn180)
  {
    moveSteps(1, -1, (WHEELS_DIST * PI / 2) * ONE_CM);
    state = forward;
  }
}

void loop() {
  readUltrasonicData();
  checkWalls();
  checkState();
  moveByState();
  Serial.println("my state: " + stateToString());
}

String stateToString()
{
  if (state == turnLeft)
    return "turnLeft";
  else if (state == turnRight)
    return "turnRight";
  else if (state == forward)
    return "forward";
  else if (state == turn180)
    return "turn180";
  else
    return "unknown state";
}
