////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib-Arduino
//
//  Copyright (c) 2014-2015, richards-tech
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of 
//  this software and associated documentation files (the "Software"), to deal in 
//  the Software without restriction, including without limitation the rights to use, 
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
//  Software, and to permit persons to whom the Software is furnished to do so, 
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all 
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//  Further modified by www.kingtidesailing.com
//
////////////////////////////////////////////////////////////////////////////

#include "Wire.h"
#include "I2Cdev.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "RTFusionRTQF.h" 
#include "CalLib.h"
#include "EEPROM.h"
#include "sonar.h"
#include "RadioCoder.h"
#include "Drone.h"
#include <Servo.h>

#define Engine_1_pin 3
#define Engine_2_pin 5
#define Engine_3_pin 6
#define Engine_4_pin 9

Drone drone(true, Engine_1_pin, Engine_2_pin, Engine_3_pin, Engine_4_pin);
RadioCoder radio;
Sonar sonarHeight(7);

RTIMU *imu;                                           // the IMU object
RTFusionRTQF fusion;                                  // the fusion object
RTIMUSettings settings;                               // the settings object

//  DISPLAY_INTERVAL sets the rate at which results are displayed

#define DISPLAY_INTERVAL  0                         // interval between pose displays

//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port

#define  SERIAL_PORT_SPEED  115200

unsigned long lastDisplay;
unsigned long lastRate;
bool ErrorCaught = false;
int sampleCount;
int Commands[] = {0,0,0,0,0};
int* pCommands = Commands;
int LastCommandMilli;
int Height;
int HeightOffset;

void setup()
{
    int errcode;
  
    Serial.begin(SERIAL_PORT_SPEED);
    Wire.begin();
    imu = RTIMU::createIMU(&settings);                        // create the imu object
  
    imu->IMUName();
    
    if ((errcode = imu->IMUInit()) < 0) {
        ErrorCaught = true;
    }
  
    imu->getCalibrationValid();

    lastDisplay = lastRate = millis();
    sampleCount = 0;

    // Slerp power controls the fusion and can be between 0 and 1
    // 0 means that only gyros are used, 1 means that only accels/compass are used
    // In-between gives the fusion mix.
    
    fusion.setSlerpPower(0.02);
    
    // use of sensors in the fusion algorithm can be controlled here
    // change any of these to false to disable that sensor
    
    fusion.setGyroEnable(true);
    fusion.setAccelEnable(true);
    fusion.setCompassEnable(true);

    HeightOffset = sonarHeight.GetHeight();
}

void loop(){
    if(ErrorCaught){
      return;
    }
        
    unsigned long now = millis();
    unsigned long delta;
    int loopCount = 1;
  
    while (imu->IMURead()) {                                // get the latest data if ready yet
        // this flushes remaining data in case we are falling behind
        if (++loopCount >= 10)
            continue;
        fusion.newIMUData(imu->getGyro(), imu->getAccel(), imu->getCompass(), imu->getTimestamp());
        sampleCount++;
        if ((delta = now - lastRate) >= 1000) {
            sampleCount = 0;
            lastRate = now;
        }
        if ((now - lastDisplay) >= DISPLAY_INTERVAL) {
            //radio communication          
            if(radio.ReceiveDecode(&pCommands)){
              if(Commands[0]==1){ //The glove did a lockdown of the system, deactivate the drone and let it fall down
                lockDrone();
              }
            } 
            if(now - lastDisplay >= 60){
                lockDrone();
            }

            lastDisplay = now;
            RTVector3 pose = fusion.getFusionPose();
            RTVector3 gyro = imu->getGyro();
            RTVector3 accel = imu->getAccel();
            float r = M_PI/180.0f;            // degrees to radians
            float d = 180.0f/M_PI;            // radians to degrees
            float roll = pose.y()*d*-1;       // left roll is negative
            float pitch = pose.x()*d;         // nose down is negative
            float yaw = pose.z()*d;           // 0 Yaw = 270 magnetic, this gives left or right up to 180 degrees
            float hdm = yaw-90;               // converts yaw to heading magnetic
            if (yaw < 90 && yaw >= -179.99) {
              hdm = yaw+270;
            }
            float ror =  gyro.y()*-1*d;       // rate of roll
            float rop = gyro.x()*d;           // rate of pitch
            float rot = gyro.z()*d;           // rate of turn
            float ax = accel.x();             // acceleration on the x-axis
            float ay = accel.y();             // acceleration on the y-axis
            float az = accel.z();             // acceleration on the z-axis
            int ts = imu->getTimestamp();     // the timestamp
            
            /*
            Serial.print("Pose: ");
            Serial.print(roll);
            Serial.print("  |  ");
            Serial.print(pitch);
            Serial.print("  |  ");
            Serial.println(hdm);
            */
            
            Height = sonarHeight.GetHeight() - HeightOffset;
            drone.WriteAllEngines(Commands[1]);
            
            /*
            Serial.print("Gyro: ");
            Serial.print(ror);
            Serial.print("  |  ");
            Serial.print(rop);
            Serial.print("  |  ");
            Serial.println(rot);
            Serial.print("Accl: ");
            Serial.print(ax);
            Serial.print("  |  ");
            Serial.print(ay);
            Serial.print("  |  ");
            Serial.println(az);
            Serial.print("Time: ");
            Serial.println(ts);
            */
            //Serial.println("----------------------------");
        }
    }
}
void lockDrone(){ //function to lockdown the drone
  int LockPower[4] = {0,0,0,0};
  drone.WriteAllEngines(*LockPower);
  while(true){
    delay(10000);
   }
}   
