#include <Stepper.h>

// change this to the number of steps on your motor
#define STEPS 200

#define ONE_REV ((360 / 5.625) * 32)
#define ONE_CM (ONE_REV / 18.2)
#define WHEELS_DIST (17.4)

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepperLeft(STEPS, 8, 10, 9, 11);
Stepper stepperRight(STEPS, 4, 6, 5, 7);

int i;

void setup() {
  // set the speed of the motor to 30 RPMs
  stepperLeft.setSpeed(200);
  stepperRight.setSpeed(200);
  i = ONE_CM * 50;
}

void loop() {
//  delay(50);
  if(i > 0){
    stepperLeft.step(1);
    stepperRight.step(-1);
    i--;
   }
   else if(i > (-(16.7 * PI / 2) * ONE_CM)){
    stepperLeft.step(1);
    stepperRight.step(1);
    i--;
   }
   else if (i > (-(50 + 16.7 * PI / 2) * ONE_CM)) {
    stepperLeft.step(1);
    stepperRight.step(-1);
    i--;
   }
}
