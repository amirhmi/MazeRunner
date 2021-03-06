#include <NewPing.h>
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
#define BEFORE_TURN (22)
#define AFTER_TURN (12)
#define TURN_DELAY (500)
#define MAJORITY_NUM (5)

#define TURN_LEFT_STEPS (((WHEELS_DIST + 0.2) * PI / 4) * ONE_CM)
#define TURN_RIGHT_STEPS (((WHEELS_DIST + 0.75) * PI / 4) * ONE_CM)
#define TURN_QUANT (10)
#define READ_ULTRASONIC_NUM (7)

enum {
  forward, turnLeft, turnRight, pause, beforeTurnLeft, afterTurnLeft, reverseGear, reverseTurnLeft, reverseTurnRight
}state;

enum {
  inter_T, inter_X, inter_left, inter_right, inter_left_fwd, inter_right_fwd
}intersection;

NewPing left_hcsr04(SR_LEFT_TRIG, SR_LEFT_ECHO, MAX_PING_DIST);
NewPing right_hcsr04(SR_RIGHT_TRIG, SR_RIGHT_ECHO, MAX_PING_DIST);
NewPing front_hcsr04(SR_FRONT_TRIG, SR_FRONT_ECHO, MAX_PING_DIST);

//Global variables
unsigned long leftDist, rightDist, frontDist;
unsigned long leftDist1, leftDist2, leftDist3, rightDist1, rightDist2, rightDist3, frontDist1, frontDist2, frontDist3;
bool leftWall, rightWall, frontWall, wasReverse;
int leftWallCount, rightWallCount, frontWallCount;
int last_heading;

int turnSteps, afterIntersectionSteps;

void setup(){
  //Serial initilization
  Serial.begin(9600);
  
  // set the speed of the motor to 30 RPMs
  stepperLeft.setSpeed(200);
  stepperRight.setSpeed(200);
  leftDist = rightDist = 5;
  frontDist = frontDist1 = MAX_PING_DIST;
  leftDist1 = rightDist1 = 5;
  leftWall = rightWall = frontWall = false;
  turnSteps  = 0;
  rightWallCount = leftWallCount = 5;
  frontWallCount = 0;
  state = forward;
  last_heading = getHeading();
}

int getHeading()
{
  int dir;
  while (1)
  {
    Serial.write("$");
    unsigned long now = millis();
    char c = '0';
    dir = 0;
    bool breakfor = false;
    for (int i = 0; i < 3; i++)
    {
      while(Serial.available() <= 0)
        if (millis() - now > 1000)
        {
          breakfor = true;
          break;
        }
      if (breakfor)
        break;
      c = Serial.read();
      dir = dir * 10 + (c - '0');
    }
    if (!breakfor)
      break;
  }
//  while(Serial.available() <= 0);
//  Serial.read();
  return dir;
}

void readUltrasonicData(int num) {
  for(int i=0; i<num; i++) {
    leftDist1 = left_hcsr04.ping_cm();
    rightDist1 = right_hcsr04.ping_cm();
    frontDist1 = front_hcsr04.ping_cm();
  
    if(leftDist1 != 0) leftDist = leftDist1;
    if(rightDist1 != 0) rightDist = rightDist1;
    if(frontDist1 != 0) frontDist = frontDist1;
    
    checkWalls();
  }

//  Serial.print("left: " + String(leftDist1));
//  Serial.print("\tright :" + String(rightDist1));
//  Serial.println("\tfront :" + String(frontDist1));
//  Serial.print("leftWall: " + String(leftWall));
//  Serial.print("\trightWall: " + String(rightWall));
//  Serial.println("\tfrontWall: " + String(frontWall));
}

void checkWalls()
{
  //checking walls
  if (rightDist < DEADEND_SIDE_DIST)
    rightWallCount ++;
  else
    rightWallCount --;

  if (leftDist < DEADEND_SIDE_DIST)
    leftWallCount ++;
  else
    leftWallCount --;

  if (frontDist < DEADEND_FWD_DIST)
    frontWallCount ++;
  else
    frontWallCount --;

  if (rightWallCount > MAJORITY_NUM)
    rightWallCount = MAJORITY_NUM;
  if (leftWallCount > MAJORITY_NUM)
    leftWallCount = MAJORITY_NUM;
  if (frontWallCount > MAJORITY_NUM)
    frontWallCount = MAJORITY_NUM;
  if (rightWallCount < 0)
    rightWallCount = 0;
  if (leftWallCount < 0)
    leftWallCount = 0;
  if (frontWallCount < 0)
    frontWallCount = 0;
  
  if (rightWallCount >= (MAJORITY_NUM + 1) / 2)
    rightWall = true;
  else
    rightWall = false;
  if (leftWallCount >= (MAJORITY_NUM + 1) / 2)
    leftWall = true;
  else
    leftWall = false;
  if (frontWallCount >= (MAJORITY_NUM + 1) / 2)
    frontWall = true;
  else
    frontWall = false;

//  Serial.print("leftWall: " + String(leftWall));
//  Serial.print("\trightWall: " + String(rightWall));
//  Serial.println("\tfrontWall: " + String(frontWall));
}

void checkState()
{
  if (state == pause) // for testing
    return;
  if (state == turnRight || state == turnLeft || state == beforeTurnLeft || state == afterTurnLeft || state == reverseGear || state == reverseTurnRight || state == reverseTurnLeft)
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

void correctHeading (int expected_heading)
{
  int my_heading = getHeading();
  expected_heading -= 180;
  my_heading -= 180;
  int heading_diff = (expected_heading + 360) - my_heading;
  if (heading_diff > 180)
    heading_diff -= 360;
  if (heading_diff < -180)
    heading_diff += 360;
  while (heading_diff > 1 || heading_diff < -1)
  {
    if (expected_heading > 90 && my_heading < -90)
      moveSteps(-1, 1, TURN_QUANT);
    else if (heading_diff < 0)
      moveSteps(-1, 1, TURN_QUANT);
    else
      moveSteps(1, -1, TURN_QUANT);
    my_heading = getHeading() - 180;
    heading_diff = (expected_heading +360) - my_heading;
    if (heading_diff > 180)
      heading_diff -= 360;
    if (heading_diff < -180)
      heading_diff += 360;
  }
}

//void turnToExpected (int expected_heading, int is_left)
//{
//  int dif = (expected_heading - getHeading() + 360) % 360;
//  while (dif > 1 || dif < -1) // 0 error
//  {
//    moveSteps(is_left, -1 * is_left, TURN_QUANT);
//    dif = (expected_heading - getHeading() + 360) % 360;
//  }
//}

void turn90left ()
{
  int expected_heading = last_heading + 90;
  expected_heading %= 360;
  delay(TURN_DELAY);
  moveSteps(1, -1, ((((WHEELS_DIST - 1) * PI / 4) * ONE_CM)));
  delay(TURN_DELAY);
  //turnToExpected (expected_heading, 1);
  correctHeading(expected_heading);
  delay(TURN_DELAY);
  last_heading = expected_heading;
}

void turn90right ()
{
  int expected_heading = last_heading + 270;
  expected_heading %= 360;
  delay(TURN_DELAY);
  moveSteps(-1, 1, ((((WHEELS_DIST - 1) * PI / 4) * ONE_CM)));
  delay(TURN_DELAY);
  //turnToExpected (expected_heading, -1);
  correctHeading(expected_heading);
  delay(TURN_DELAY);
  last_heading = expected_heading;
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
    moveSteps(1, 1, (BEFORE_TURN - 5) * ONE_CM);
    readUltrasonicData(1);
    if (frontDist >= 35)
      moveSteps(1, 1, 5 * ONE_CM);
    state = turnLeft;
    readUltrasonicData(1);
    while (frontDist < 35 && frontDist > 3)
    {
      if (frontDist1 == 0)
        break;
      delay(50);
      moveSteps(1, 1, ONE_CM);
      readUltrasonicData(1);
    }
  }
  else if (state == afterTurnLeft)
  {
    moveSteps(1, 1, (AFTER_TURN) * ONE_CM);
    state = forward;
    afterIntersectionSteps += (AFTER_TURN) * ONE_CM;
  }
  else if (state == turnLeft)
  {
    turn90left();
    state = afterTurnLeft;
    afterIntersectionSteps = 0;
    wasReverse = false;
  }
  else if (state == turnRight)
  {
    turn90right();
    state = forward;
    afterIntersectionSteps = 0;
  }
  else if (state == reverseGear)
  {
    delay(TURN_DELAY);
    moveSteps(-1, -1, afterIntersectionSteps + ONE_CM * 5);
    delay(TURN_DELAY);
    readUltrasonicData(READ_ULTRASONIC_NUM);
    if (!rightWall)
      state = reverseTurnRight;
    else if (!leftWall)
      state = reverseTurnLeft;
//    else
//      state = pause;
  }
  else if (state == reverseTurnRight)
  {
    turn90right();
    state = afterTurnLeft;
    afterIntersectionSteps = 0;
    wasReverse = true;
  }
  else if (state == reverseTurnLeft)
  {
    turn90left();
    state = forward;
    moveSteps(-1, -1, 8 * ONE_CM);
    readUltrasonicData(READ_ULTRASONIC_NUM);
    if (!leftWall)
      state = turnLeft;
    afterIntersectionSteps = 0;
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
  readUltrasonicData(5);
  checkState();
  moveByState();
  checkIntersection();
  correctHeading(last_heading);
  //Serial.println("state = " + stateToString());
}

String stateToString()
{
  if (state == turnLeft)
    return "turnLeft";
  else if (state == turnRight)
    return "turnRight";
  else if (state == forward)
    return "forward";
  else if (state == pause)
    return "pause";
  else if (state == beforeTurnLeft)
    return "beforeTurnLeft";
  else if (state == afterTurnLeft)
    return "afterTurnLeft";
  else if (state == reverseGear)
    return "reverseGear";
  else if (state == reverseTurnLeft)
    return "reverseTurnLeft";
  else if (state == reverseTurnRight)
    return "reverseTurnRight";
  else
    return "unknown state";
}
