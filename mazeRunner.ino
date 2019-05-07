#include <Stepper.h>

#define STEPS (200)

#define ONE_REV ((360 / 5.625) * 32)
#define ONE_CM (ONE_REV / 18.2)
#define WHEELS_DIST (16.7)
#define SPEED (200)
#define DISTANCE (50)


#define STEP_PER_TICK (1)
#define LF (1 * STEP_PER_TICK)
#define LB (-1 * STEP_PER_TICK)
#define RF (-1 * STEP_PER_TICK)
#define RB (1 * STEP_PER_TICK)

Stepper leftMotor(STEPS, 8, 10, 9, 11);
Stepper rightMotor(STEPS, 4, 6, 5, 7);

enum {forwardMove, rotate180, headingCorrection} state;
int movedSteps;

void setup() {
  // set the speed of the motor to 30 RPMs
  leftMotor.setSpeed(SPEED);
  rightMotor.setSpeed(SPEED);
  movedSteps = 0;
  state = forwardMove;
}

void loop() {
  if (state == forwardMove) {
    if (movedSteps < ONE_CM * DISTANCE)
    {
      leftMotor.step(LF);
      rightMotor.step(RF);
      movedSteps += STEP_PER_TICK;
    }
    else
    {
      state = rotate180;
      movedSteps = 0;
    }
  }
  else if (state == rotate180) {
    if (movedSteps < (ONE_CM * WHEELS_DIST * (PI/2)))
    {
      leftMotor.step(LF);
      rightMotor.step(RB);
      movedSteps += STEP_PER_TICK;
    }
    else
    {
      state = forwardMove;
      movedSteps = 0;
    }
  }
}
