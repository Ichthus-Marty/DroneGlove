/*************************************************************
project: <type project name here>
author: <type your name here>
description: <type what this file does>
*************************************************************/

#include <arduino.h>
#include "ProjectD_main.h"

#include <C:\Arduino\libraries\Wire\Wire.h>
#include <sonar.h>
#include <gam_sensors.h>

GAM_Sensors GAM_Test;
Sonar Sonar_Test(7);

void setup()
{
	Serial.begin(9600);
	
}

void loop()
{
	int Accel[3];
	Serial.print(Sonar_Test.GetHeight());
	Serial.println();
	
	GAM_Test.GAM_Accel(Accel); //Update Accel[]
	Serial.print(Accel[0]);
	Serial.print(Accel[1]);
	Serial.print(Accel[2]);
	Serial.println();
	
	delay(1000);
}