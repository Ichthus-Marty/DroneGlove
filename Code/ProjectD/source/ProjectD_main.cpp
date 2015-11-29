/*************************************************************
project: <type project name here>
author: <type your name here>
description: <type what this file does>
*************************************************************/

#include <arduino.h>
#include "ProjectD_main.h"

//#include <C:\Arduino\libraries\Wire\Wire.h>
#include <Wire.h>
#include <sonar.h>
#include <gam_sensors.h>

/*GAM_Sensors GAM_Test;
Sonar Sonar_Test(7);
*/
GAM_Sensors* pGAM_Test;

void setup()
{
	pGAM_Test = new GAM_Sensors;
	
	Serial.begin(9600);
	
}

void loop()
{

	int Accel[3];
	/*
	Accel[0] = 1;
	Accel[1] = 2;
	Accel[2] = 3;
	*/
	
	//Serial.print(Sonar_Test.GetHeight());
	//Serial.println();
	
	pGAM_Test->GAM_Accel(Accel); //Update Accel[]
	
	//int Accel[3];
	//Accel[0] = 1;
	//Accel[1] = 2;
	//Accel[2] = 3;
	
	Serial.print(Accel[0]);
	Serial.println();
	Serial.print(Accel[1]);
	Serial.println();
	Serial.print(Accel[2]);
	Serial.println();
	
	delay(1000);
}