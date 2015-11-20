/*************************************************************
project: <type project name here>
author: <type your name here>
description: <type what this file does>
*************************************************************/

#include "Arduino.h"
#include "sonar.h"

Sonar::Sonar(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

long Sonar::GetHeight()
{
   // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(_pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(_pin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(_pin, INPUT);
  _duration = pulseIn(_pin, HIGH);

  // convert the time into a distance
  _distance = _duration / 29 / 2;
	
	return _distance; 
}
