#include <hcsr04.h>
#include <Stepper.h>


// Stepper motor defenitions and constructor 
#define STEPS 200
#define ONE_REV ((360 / 5.625) * 32)
#define ONE_CM (ONE_REV / 18.2)
#define WHEELS_DIST (16.7)

#define LEFT_FRONT (1)
#define LEFT_BACK (-1)
#define RIGHT_FRONT (-1)
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

HCSR04 left_hcsr04(SR_LEFT_TRIG, SR_LEFT_ECHO, 20, 4000);
HCSR04 right_hcsr04(SR_RIGHT_TRIG, SR_RIGHT_ECHO, 20, 4000);
HCSR04 front_hcsr04(SR_FRONT_TRIG, SR_FRONT_ECHO, 20, 4000);

int leftDist, rightDist, frontDist;

void setup(){
  //Serial initilization
  Serial.begin(9600);
  
  // set the speed of the motor to 30 RPMs
  stepperLeft.setSpeed(200);
  stepperRight.setSpeed(200);
}

void loop() {

  leftDist = left_hcsr04.distanceInMillimeters();
  rightDist = right_hcsr04.distanceInMillimeters();
  frontDist = front_hcsr04.distanceInMillimeters();

  stepperRight.step(RIGHT_FRONT);
  stepperLeft.step(LEFT_FRONT);

//  while(frontDist < 100) {
//    frontDist = front_hcsr04.distanceInMillimeters();
//  }

//     Output the distance in mm
//  Serial.print("left: " + String(leftDist));
//  Serial.print("\tright :" + String(rightDist));
//  Serial.println("\tfront :" + String(frontDist));
//  delay(250);
}
