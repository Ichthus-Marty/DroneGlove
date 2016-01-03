/*************************************************************
project: <type project name here>
author: <type your name here>
description: <type what this file does>
*************************************************************/

#include "Demo_Radio_Drone_main.h"
/*
  Serial Event example

 When new serial data arrives, this sketch adds it to a String.
 When a newline is received, the loop prints the string and
 clears it.

 A good test for this is to try it with a GPS receiver
 that sends out NMEA 0183 sentences.

 Created 9 May 2011
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/SerialEvent

 */

void setup() {  
	pinMode(8, OUTPUT); 
	pinMode(13, OUTPUT);  
	digitalWrite(8, HIGH);  
	Serial.begin(115200); 
}
void loop() {  
	if (Serial.available()){    
		char c = Serial.read();    
		if (c == '0'){      
			digitalWrite(13, LOW);    
		} 
		else if (c == '1') {      
			digitalWrite(13, HIGH);    
		}  
	}    
}
