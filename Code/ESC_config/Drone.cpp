/*************************************************************
project: <type project name here>
author: <type your name here>
description: <type what this file does>
*************************************************************/

#include "Arduino.h"
#include "Drone.h"
#include <Servo.h>

Servo Servo_Engine1;  // create servo object to control the front left engine
Servo Servo_Engine2;  // create servo object to control the front right engine
Servo Servo_Engine3;  // create servo object to control the back right engine
Servo Servo_Engine4;  // create servo object to control the back left engine

Drone::Drone(bool Attach, int pinEngine1, int pinEngine2, int pinEngine3, int pinEngine4){
  _pinEngine1 = pinEngine1;
  _pinEngine2 = pinEngine2;
  _pinEngine3 = pinEngine3;
  _pinEngine4 = pinEngine4;

  if(Attach){ //Only attach the engines when instructed to do so (we don't want to attach the engines when we are going to enter the setup mode of the speedcontrollers later on
    Servo_Engine1.attach(_pinEngine1);
    Servo_Engine2.attach(_pinEngine2);
    Servo_Engine3.attach(_pinEngine3);
    Servo_Engine4.attach(_pinEngine4);
  };
  
}

void Drone::WriteSingleEngine(int EngineNum, int Throttle){
  switch (EngineNum) {
    case 1:
      Servo_Engine1.writeMicroseconds(Throttle);
      break;
    case 2:
      Servo_Engine2.writeMicroseconds(Throttle);
      break;
    case 3:
      Servo_Engine3.writeMicroseconds(Throttle);
      break;
    case 4:
      Servo_Engine4.writeMicroseconds(Throttle);
      break;
  };
}
void Drone::WriteSepEngines(int ThrottleArray[4]){
  Servo_Engine1.writeMicroseconds(ThrottleArray[0]);
  Servo_Engine2.writeMicroseconds(ThrottleArray[1]);
  Servo_Engine3.writeMicroseconds(ThrottleArray[2]);
  Servo_Engine4.writeMicroseconds(ThrottleArray[3]);
}
void Drone::WriteAllEngines(int Throttle){
  Servo_Engine1.writeMicroseconds(Throttle);
  Servo_Engine2.writeMicroseconds(Throttle);
  Servo_Engine3.writeMicroseconds(Throttle);
  Servo_Engine4.writeMicroseconds(Throttle);
}
void Drone::Write_X_Engines(int Throttle){
  Servo_Engine2.writeMicroseconds(Throttle);
  Servo_Engine3.writeMicroseconds(Throttle);
}
void Drone::Write_Y_Engines(int Throttle){
  Servo_Engine1.writeMicroseconds(Throttle);
  Servo_Engine2.writeMicroseconds(Throttle);
}
void Drone::ConfigEngines(int lowValue, int highValue){
  Serial.println("Callibration of the drone's ECS' begins...");
  
  Servo_Engine1.attach(_pinEngine1);
  Servo_Engine2.attach(_pinEngine2);
  Servo_Engine3.attach(_pinEngine3);
  Servo_Engine4.attach(_pinEngine4);

  Serial.println("Now writing maximum output.");
  Serial.println("Turn on power the source, then wait 2 seconds for the disco and press any key.");
  Servo_Engine1.writeMicroseconds(highValue);
  Servo_Engine2.writeMicroseconds(highValue);
  Servo_Engine3.writeMicroseconds(highValue);
  Servo_Engine4.writeMicroseconds(highValue);

  // Wait for input
  while (!Serial.available());
  Serial.read();

  // Send min output
  Serial.println("Sending minimum output");
  Servo_Engine1.writeMicroseconds(lowValue);
  Servo_Engine2.writeMicroseconds(lowValue);
  Servo_Engine3.writeMicroseconds(lowValue);
  Servo_Engine4.writeMicroseconds(lowValue);

  Serial.println("---------------------------------------------------");
  Serial.println("Detaching ECS' now");

  Servo_Engine1.detach();
  Servo_Engine2.detach();
  Servo_Engine3.detach();
  Servo_Engine4.detach();

  Serial.println("Callibration of ESC' completed"); 
}
