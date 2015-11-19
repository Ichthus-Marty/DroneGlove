//-------------------------------------------------------------------
#ifndef __agm_test_main_H__
#define __agm_test_main_H__
//-------------------------------------------------------------------
 
#include <arduino.h>
#include <Wire.h>
#include <twi.h>
//#include <Wire.h>
//#include <twi.h>
 
//-------------------------------------------------------------------
 
//-------------------------------------------------------------------
 
// Put yout declarations here
 
//-------------------------------------------------------------------
 
//===================================================================
// -> DO NOT WRITE ANYTHING BETWEEN HERE...
// 		This section is reserved for automated code generation
// 		This process tries to detect all user-created
// 		functions in main_sketch.cpp, and inject their
// 		declarations into this file.
// 		If you do not want to use this automated process,
//		simply delete the lines below, with "&MM_DECLA" text
//===================================================================
//---- DO NOT DELETE THIS LINE -- @MM_DECLA_BEG@---------------------
int MPU9150_writeSensor(int addr,int data);
int MPU9150_readSensor(int addr);
int MPU9150_readSensor(int addrL, int addrH);
void MPU9150_setupCompass();
void loop();
void setup();
//---- DO NOT DELETE THIS LINE -- @MM_DECLA_END@---------------------
// -> ...AND HERE. This space is reserved for automated code generation!
//===================================================================
 
 
//-------------------------------------------------------------------
#endif
//-------------------------------------------------------------------
 
 
