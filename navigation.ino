#include <Stepper.h>

// change this to the number of steps on your motor
#define STEPS 200

#define ONE_REV ((360 / 5.625) * 32)
#define ONE_CM (ONE_REV / 18.2)
#define WHEELS_DIST (17.4)

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper1(STEPS, 8, 10, 9, 11);
Stepper stepper2(STEPS, 4, 6, 5, 7);

int i;

void setup() {
  // set the speed of the motor to 30 RPMs
  stepper1.setSpeed(200);
  stepper2.setSpeed(200);
  i = ONE_CM * 50;
}

void loop() {
//  delay(50);
  if(i > 0){
    stepper1.step(1);
    stepper2.step(-1);
    i--;
   }
   else if(i > (-(16.7 * PI / 2) * ONE_CM)){
    stepper1.step(1);
    stepper2.step(1);
    i--;
   }
   else if (i > (-(50 + 16.7 * PI / 2) * ONE_CM)) {
    stepper1.step(1);
    stepper2.step(-1);
    i--;
   }
}
