/*************************************************************
project: <type project name here>
author: <type your name here>
description: <type what this file does>
*************************************************************/

#include <arduino.h>
#include "ProjectD_main.h"

#include <Morse.h>

Sonar test(7);

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	Serial.print(test.GetHeight());
	Serial.println();
	delay(1000);
}