#include "Drone.h"
#include <Servo.h>

#define Engine_1_pin 3
#define Engine_2_pin 5
#define Engine_3_pin 6
#define Engine_4_pin 9

Drone drone(false, Engine_1_pin, Engine_2_pin, Engine_3_pin, Engine_4_pin);

void setup(){
  Serial.begin(115200);
  drone.ConfigEngines(1000, 2000);
}
void loop(){
  
}

