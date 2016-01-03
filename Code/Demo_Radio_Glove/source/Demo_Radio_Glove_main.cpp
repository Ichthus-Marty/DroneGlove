/*************************************************************
project: <type project name here>
author: <type your name here>
description: <type what this file does>
*************************************************************/

#include "Demo_Radio_Glove_main.h"

void setup() {  
	pinMode(8, OUTPUT);  
	digitalWrite(8, HIGH); 
	Serial.begin(115200);  
}
void loop(){
	Serial.print('1');
	
	delay(1000);
}
