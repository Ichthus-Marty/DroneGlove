#include "Servo.h"


/*Follow the next steps to callibrate the speedcontrollers:
 * -DO NOT attach the speedcontrollers to the battery until this sketch tells you to do so.
 * -Wire the signal wires of the speedcontrollers to digitals pins 3, 5, 6 and 9 (one pin for each speedcontroller)
 * -Wire the ground wires of the speedcontrollers to a ground pin on the Arduino
 * -Upload this sketch to the Arduino and run in
 * -Follow the instructions in the serial monitor
 */

Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;

void setup() {
  Serial.begin(9600);

  //Attach the engines, wired up to digital pins 3, 5, 6 and 9
  myservo1.attach(3);
  myservo2.attach(5);
  myservo3.attach(6);
  myservo4.attach(9);
  
  Serial.println("WRITING HIGH VALUE, CONNECT THE SPEEDCONTROLLERS TO THE BATTERY");
  
  myservo1.writeMicroseconds(2000);
  myservo2.writeMicroseconds(2000);
  myservo3.writeMicroseconds(2000);
  myservo4.writeMicroseconds(2000);
  
  Serial.println("Type something when the speedcontrollers stops beeping");
  while(!Serial.available());

  Serial.println("WRITING LOW VALUE");

  myservo1.writeMicroseconds(1000);
  myservo2.writeMicroseconds(1000);
  myservo3.writeMicroseconds(1000);
  myservo4.writeMicroseconds(1000);

  Serial.println("Callibration of speedcontrollers done");
  Serial.println("Detach the speedcontrollers from the battery when they stoppen beeping");
}

void loop() {
  //Noting to do here, we callibrate only once
}
