#include <hcsr04.h>

#define SR_LEFT_TRIG A1
#define SR_LEFT_ECHO A0

#define SR_RIGHT_TRIG 2
#define SR_RIGHT_ECHO 3

#define SR_FRONT_TRIG 12
#define SR_FRONT_ECHO 13



HCSR04 left_hcsr04(SR_LEFT_TRIG, SR_LEFT_ECHO, 20, 4000);
HCSR04 right_hcsr04(SR_RIGHT_TRIG, SR_RIGHT_ECHO, 20, 4000);
HCSR04 front_hcsr04(SR_FRONT_TRIG, SR_FRONT_ECHO, 20, 4000);

void setup(){
    Serial.begin(9600);
}

void loop() {

    // Output the distance in mm
  Serial.print("left :" + left_hcsr04.distanceInMillimeters());
  Serial.print("right :" + right_hcsr04.distanceInMillimeters());
  Serial.println("front :" + front_hcsr04.distanceInMillimeters());
  delay(250);
}
