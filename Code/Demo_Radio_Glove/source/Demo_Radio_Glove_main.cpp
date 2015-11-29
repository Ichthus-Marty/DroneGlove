/*************************************************************
project: <type project name here>
author: <type your name here>
description: <type what this file does>
*************************************************************/

#include "Demo_Radio_Glove_main.h"

void setup() {                
  Serial.begin(9600);  
}

void loop() {
	Serial.write("Hello World!"); 
	delay(1000);
}
