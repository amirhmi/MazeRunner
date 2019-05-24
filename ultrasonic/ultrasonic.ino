#include <hcsr04.h>
#include <Stepper.h>

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

// Ultrasonic defenitions and constructor 
#define SR_LEFT_TRIG A1
#define SR_LEFT_ECHO A0

#define SR_RIGHT_TRIG 2
#define SR_RIGHT_ECHO 3

#define SR_FRONT_TRIG 12
#define SR_FRONT_ECHO 13

#define MAX_PING_DIST (3500)
#define MIN_PING_DIST (20)
#define DEADEND_DIST (150)


enum {
  forward, turn180, turnLeft, turnRight
}state;

HCSR04 left_hcsr04(SR_LEFT_TRIG, SR_LEFT_ECHO, MIN_PING_DIST, MAX_PING_DIST);
HCSR04 right_hcsr04(SR_RIGHT_TRIG, SR_RIGHT_ECHO, MIN_PING_DIST, MAX_PING_DIST);
HCSR04 front_hcsr04(SR_FRONT_TRIG, SR_FRONT_ECHO, MIN_PING_DIST, MAX_PING_DIST);

//Timer for SR04
#define SR_TIMER (500)

//Global variables
unsigned long leftDist, rightDist, frontDist;
bool leftWall, rightWall, frontWall;

void setup(){
  //Serial initilization
  Serial.begin(9600);
  
  // set the speed of the motor to 30 RPMs
  stepperLeft.setSpeed(200);
  stepperRight.setSpeed(200);
  leftDist = rightDist = frontDist = MAX_PING_DIST;
  leftWall = rightWall = frontWall = false;
}

void readUltrasonicData(void) {
  leftDist = left_hcsr04.distanceInMillimeters();
  rightDist = right_hcsr04.distanceInMillimeters();
  frontDist = front_hcsr04.distanceInMillimeters();
//  Serial.print("left: " + String(leftDist));
//  Serial.print("\tright :" + String(rightDist));
//  Serial.println("\tfront :" + String(frontDist));
}

void checkWalls()
{
  //checking walls
  if (rightDist < DEADEND_DIST)
    rightWall = true;
  else
    rightWall = false;

  if (leftDist < DEADEND_DIST)
    leftWall = true;
  else
    leftWall = false;

  if (frontDist < DEADEND_DIST)
    frontWall = true;
  else
    frontWall = false;

//  Serial.print("leftWall: " + String(leftWall));
//  Serial.print("\trightWall: " + String(rightWall));
//  Serial.println("\tfrontWall: " + String(frontWall));
}

void checkState()
{
  if (!leftWall)
    state = turnLeft;
  else if (!frontWall)
    state = forward;
  else if (!rightWall)
    state = turnRight;
  else
    state = turn180;
//  Serial.println(String(state));
}

void moveSteps(int rightMove, int leftMove)
{
  for (int i = 0; i < STEP_NUM; i++)
  {
    stepperLeft.step(LEFT_FORWARD * leftMove);
    stepperRight.step(RIGHT_FORWARD * rightMove);
  }
}

void moveByState()
{
  if (state == forward)
    moveSteps(1, 1);
}

void loop() {
  readUltrasonicData();
  checkWalls();
  checkState();
  moveByState();
}
