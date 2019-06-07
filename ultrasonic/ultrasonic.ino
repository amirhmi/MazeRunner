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
#define DEADEND_FWD_DIST (5)
#define DEADEND_SIDE_DIST (20)
#define BEFORE_TURN (23)
#define AFTER_TURN (13)
#define TURN_DELAY (250)

#define TURN_LEFT_STEPS (((WHEELS_DIST + 0.15) * PI / 4) * ONE_CM)
#define TURN_RIGHT_STEPS (((WHEELS_DIST + 0.75) * PI / 4) * ONE_CM)


enum {
  forward, turn180, turnLeft, turnRight, pause, beforeTurnLeft, afterTurnLeft, reverseGear, reverseTurnLeft, reverseTurnRight
}state;

enum {
  inter_T, inter_X, inter_left, inter_right, inter_left_fwd, inter_right_fwd
}intersection;

NewPing left_hcsr04(SR_LEFT_TRIG, SR_LEFT_ECHO, MAX_PING_DIST);
NewPing right_hcsr04(SR_RIGHT_TRIG, SR_RIGHT_ECHO, MAX_PING_DIST);
NewPing front_hcsr04(SR_FRONT_TRIG, SR_FRONT_ECHO, MAX_PING_DIST);
//HCSR04 left_hcsr04(SR_LEFT_TRIG, SR_LEFT_ECHO, MIN_PING_DIST, MAX_PING_DIST);
//HCSR04 right_hcsr04(SR_RIGHT_TRIG, SR_RIGHT_ECHO, MIN_PING_DIST, MAX_PING_DIST);
//HCSR04 front_hcsr04(SR_FRONT_TRIG, SR_FRONT_ECHO, MIN_PING_DIST, MAX_PING_DIST);

//Global variables
unsigned long leftDist, rightDist, frontDist;
unsigned long leftDist1, leftDist2, leftDist3, rightDist1, rightDist2, rightDist3, frontDist1, frontDist2, frontDist3;
bool leftWall, rightWall, frontWall;

int turnSteps, afterIntersectionSteps;

void setup(){
  //Serial initilization
  Serial.begin(9600);
  
  // set the speed of the motor to 30 RPMs
  stepperLeft.setSpeed(200);
  stepperRight.setSpeed(200);
  leftDist = rightDist = 5;
  frontDist = MAX_PING_DIST;
  leftWall = rightWall = frontWall = false;
  turnSteps  = 0;
  state = forward;
}

void readUltrasonicData(void) {
  leftDist1 = left_hcsr04.ping_cm();
  rightDist1 = right_hcsr04.ping_cm();
  frontDist1 = front_hcsr04.ping_cm();

  if(leftDist1 != 0) leftDist = leftDist1;
  if(rightDist1 != 0) rightDist = rightDist1;
  if(frontDist1 != 0) frontDist = frontDist1;
  
//  //calculate sensor average
//  leftDist2 = leftDist1;
//  leftDist3 = leftDist2;
//  leftDist1 = leftDist;
//  leftDist = (leftDist1 + leftDist2 + leftDist3 ) / 3;
//  rightDist2 = rightDist1;
//  rightDist3 = rightDist2;
//  rightDist1 = rightDist;
//  rightDist = (rightDist1 + rightDist2 + rightDist3 ) / 3;
//  frontDist2 = frontDist1;
//  frontDist3 = frontDist2;
//  frontDist1 = frontDist;
//  frontDist = (frontDist1 + frontDist2 + frontDist3 ) / 3;
//  Serial.print("left: " + String(leftDist1));
//  Serial.print("\tright :" + String(rightDist1));
//  Serial.println("\tfront :" + String(frontDist1));
}

void checkWalls()
{
  //checking walls
  if (rightDist < DEADEND_SIDE_DIST)
    rightWall = true;
  else
    rightWall = false;

  if (leftDist < DEADEND_SIDE_DIST)
    leftWall = true;
  else
    leftWall = false;

  if (frontDist < DEADEND_FWD_DIST)
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
  if (state == pause) // for testing
    return;
  if (state == turnRight || state == turnLeft || state == beforeTurnLeft || state == afterTurnLeft || state == reverseGear || state == reverseTurnRight || state == reverseTurnLeft || state == turn180)
    return;
  state = forward;
  if (!leftWall)
    state = beforeTurnLeft;
  else if (!frontWall)
    state = forward;
  else if (!rightWall)
    state = turnRight;
  else
    state = reverseGear;
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
  if (state == forward)
  {
      moveSteps(1, 1, STEP_NUM);
      afterIntersectionSteps += STEP_NUM;
  }
  else if (state == beforeTurnLeft)
  {
    moveSteps(1, 1, (BEFORE_TURN) * ONE_CM);
    state = turnLeft;
  }
  else if (state == afterTurnLeft)
  {
    moveSteps(1, 1, (AFTER_TURN) * ONE_CM);
    state = forward;
    afterIntersectionSteps += (AFTER_TURN) * ONE_CM;
  }
  else if (state == turnLeft)
  {
    delay(TURN_DELAY);
    moveSteps(1, -1, TURN_LEFT_STEPS);
    delay(TURN_DELAY);
    state = afterTurnLeft;
    afterIntersectionSteps = 0;
  }
  else if (state == turnRight)
  {
    delay(TURN_DELAY);
    moveSteps(-1, 1, TURN_RIGHT_STEPS);
    delay(TURN_DELAY);
    state = forward;
    afterIntersectionSteps = 0;
  }
  else if (state == reverseGear)
  {
    delay(TURN_DELAY);
    moveSteps(-1, -1, afterIntersectionSteps + ONE_CM * 9);
    delay(TURN_DELAY);
    readUltrasonicData();
    checkWalls();
    if (!rightWall)
      state = reverseTurnRight;
    else if (!leftWall)
      state = reverseTurnLeft;
    else
      state = pause;
//    if (intersection == inter_X || intersection == inter_T || intersection == inter_right_fwd || intersection == inter_right)
//      state = reverseTurnRight;
//    else if (intersection == inter_left_fwd)
//      state = turn180;
//    else if (intersection == inter_left)
//      state = reverseTurnLeft;
//    else
//      state = pause;
  }
  else if (state == reverseTurnRight)
  {
    delay(TURN_DELAY);
    moveSteps(-1, 1, TURN_RIGHT_STEPS);
    delay(TURN_DELAY);
    state = afterTurnLeft;
  }
  else if (state == reverseTurnLeft)
  {
    delay(TURN_DELAY);
    moveSteps(1, -1, TURN_LEFT_STEPS);
    delay(TURN_DELAY);
    state = forward;
    moveSteps(-1, -1, 8 * ONE_CM);
    readUltrasonicData();
    checkWalls();
    if (!leftWall)
      state = turnLeft;
  }
//  else if (state == turn180)
//  {
//    moveSteps(1, -1, ((WHEELS_DIST - 1) * PI / 2) * ONE_CM);
//    state = forward;
//  }
}

void checkIntersection ()
{
  if (rightWall && leftWall)
    return;
  if (!rightWall && !frontWall && !leftWall)
    intersection = inter_X;
  else if (!rightWall && !leftWall)
    intersection = inter_T;
  else if (!rightWall && !frontWall)
    intersection = inter_right_fwd;
  else if (!leftWall && !frontWall)
    intersection = inter_left_fwd;
  else if (!rightWall)
    intersection = inter_right;
  else if (!leftWall)
    intersection = inter_left;
}

void loop() {
  readUltrasonicData();
  checkWalls();
  checkState();
  moveByState();
  checkIntersection();
}

String stateToString()
{
  if (state == turnLeft)
    return "turnLeft";
  else if (state == turnRight)
    return "turnRight";
  else if (state == forward)
    return "forward";
  else
    return "unknown state";
}
