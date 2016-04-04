#include "Wire.h"
#include "I2Cdev.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "RTFusionRTQF.h" 
#include "CalLib.h"
#include "EEPROM.h"
#include "sonar.h"
#include "RadioCoder.h"
#include <Servo.h>

/**************WARNING**************/
//Defining LIVE (by uncommenting the LIVE variable below) WILL ARM AND ACTIVATE THE DRONE WHEN POWERED UP!!
/**************WARNING**************/
//#define LIVE

#define DEBUG_POWER
#define DEBUG

RTIMU *imu;                                           // the IMU object
RTFusionRTQF fusion;                                  // the fusion object
RTIMUSettings settings;                               // the settings object
Sonar SonarHeight(7);                                 // The sonar object
RadioCoder Radio;                                     // Radio object for wireless communication

Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;

#define rollOffset  0.47
#define pitchOffset -2.25
#define sonarOffset 3.0
#define warmUpDelay 15000

//  DISPLAY_INTERVAL sets the rate at which results are displayed

#define DISPLAY_INTERVAL  20                         // interval between pose displays

//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port

#define  SERIAL_PORT_SPEED  115200

//  Variables for the IMU
unsigned long lastDisplay;
unsigned long lastRate;
bool ErrorCaught = false;
int sampleCount;

// Sonar variable
int Height;

// Radio variables
int Rec_Instructs[] = {0,0,0,0,0};      //Array to hold received intstuctions
int* pRI = Rec_Instructs;               //Pointer to array above
int RadioPower;                         //Holds the pRI[1] value, used multiple times in the PI
unsigned long timeLastCommand = 0;
bool runFlag = false;

//PI variabales
float Cp = 0.0; //Constant for the P
float Ci = 0.00000001; //Constant for the I 
float Error_Pitch = 0;
float Error_Roll =0 ;
float errSum_Pitch =0 ;
float errSum_Roll = 0;
float LastPI = 0;


//Drone variables
int Power[4] = {0,0,0,0}; // Array for the data output per engine.

void setup()
{
    #ifdef DEBUG
      Serial.println("SETTING UP...");
    #endif
    int errcode;
    
    pinMode(8,OUTPUT);
    digitalWrite(8, HIGH);
  
    Serial.begin(SERIAL_PORT_SPEED);
    Wire.begin();
    imu = RTIMU::createIMU(&settings);                        // create the imu object
  
    imu->IMUName();
    
    if ((errcode = imu->IMUInit()) < 0) {
        ErrorCaught = true;
        //Serial.print("Failed to init IMU: "); Serial.println(errcode);
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

    /*
    Serial.println("Units:");
    Serial.println("Pose: Roll, Pitch, Magnetic Heading (measured in degrees)");
    Serial.println("Gyro: Rate of Roll, Pitch, and Yaw (measured in degrees per second)");
    Serial.println("Accl: Acceleration in X, Y, and Z (measured in g's, with 1 g equal to 9.8 meters per second squared)");
    Serial.println("Time: Timestamp (measured in milliseconds from when the MPU-9150 was initiated)");
    Serial.println("----------------------------");
    */

    myservo1.attach(3);
    myservo2.attach(5); 
    delay(100);
    myservo3.attach(6);
    myservo4.attach(9);
    delay(1000);
    
    myservo1.writeMicroseconds(0);
    myservo2.writeMicroseconds(0);
    delay(100);
    myservo3.writeMicroseconds(0);
    myservo4.writeMicroseconds(0);
    delay(5000);
    #ifdef DEBUG
      Serial.println("SETUP DONE");
    #endif

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
     
          RTVector3 pose = fusion.getFusionPose();
          RTVector3 gyro = imu->getGyro();
          RTVector3 accel = imu->getAccel();
          float r = M_PI/180.0f;            // degrees to radians
          float d = 180.0f/M_PI;            // radians to degrees
          float roll = pose.y()*d*-1 - rollOffset;        // left roll is negative
          float pitch = pose.x()*d - pitchOffset;         // nose down is negative
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

          //Serial.print("Pose: ");
          
          Height = SonarHeight.GetHeight()- sonarOffset;
          
          /*
          Serial.print(roll);
          Serial.print("  |  ");
          Serial.print(pitch);
          Serial.print("  |  ");
          Serial.print(hdm);
          Serial.print("  |  ");
         
          Serial.println(Height);
          */

          
          
          if(Radio.ReceiveDecode(&pRI)){
            timeLastCommand = now;
            RadioPower = pRI[1]*10;
           }
           if(now - timeLastCommand >= 1000 && runFlag == true){
              LockDrone();
            }  

           if(now<warmUpDelay){
              #ifdef DEBUG
                Serial.println("warmUpDelay");
              #endif
              return;
           }

           runFlag = true;
           
           

            /*NOTE TO SELF:
             * 
             * ONLY USE THE PI OF THE PID CONTROLLER. 
             * D WILL MAKE DEBUGGING A PAIN IN THE *SS
             * 
             */

            
            //******************************************PID********************************************************
            Power[0] = 0;// + Ci*errSum_Pitch);
            Power[1] = 0;// + Ci*errSum_Pitch);
            Power[2] = 0;// + Ci*errSum_Pitch);
            Power[3] = 0;// + Ci*errSum_Pitch);
            
            //**********************PITCH******************************************
            /*
            //Calculate pitch P
            Error_Pitch = pRI[3] - pitch; 
            Serial.println("*******Error_Pitch");
            Serial.println(Error_Pitch);

            //Calculate pitch I
            /*
            errSum_Pitch += (Error_Pitch * (now-LastPI));
            Serial.println("*******now");
            Serial.println(now);
            Serial.println("*******LastPI");
            Serial.println(LastPI);
            
            Serial.println("*******errSum_Pitch");
            Serial.println(errSum_Pitch);
            */

            //Update Power array
            /*
            Power[0] += (Cp*Error_Pitch);// + Ci*errSum_Pitch);
            Power[1] += (Cp*Error_Pitch);// + Ci*errSum_Pitch);
            Power[2] -= (Cp*Error_Pitch);// + Ci*errSum_Pitch);
            Power[3] -= (Cp*Error_Pitch);// + Ci*errSum_Pitch);
            */

            //**********************ROLL******************************************
            
            //Calculate roll P
            Error_Roll = pRI[4] - roll; 

            /*
            //Calculate roll I
            errSum_Roll += (Error_Roll * (now-LastPI));
            */

            //Update Power array
            Power[0] += (Cp*Error_Roll);// + Ci*errSum_Roll);
            Power[1] += (Cp*Error_Roll);// + Ci*errSum_Roll);
            Power[2] -= (Cp*Error_Roll);// + Ci*errSum_Roll);
            Power[3] -= (Cp*Error_Roll);// + Ci*errSum_Roll);
/*
            #ifdef DEBUG_POWER
            Serial.println("*******POWER");
            for(int i=0; i<4;i++){
              Serial.println(1000+Power[i]*RadioPower);
            }
            Serial.println("*************************************");
            #endif
*/            

            for(int i=0; i<4;i++){
              Power[i] = 1000+RadioPower+Power[i]*RadioPower;
              if(Power[i]>2000){
                Power[i] = 2000;
              }
              if(Power[i]<1000){
                Power[i] = 1000;
              }
            }
            
            #ifdef DEBUG_POWER
            Serial.println("*******POWER");
            for(int i=0; i<4;i++){
              Serial.println(Power[i]);
            }
            Serial.println("*************************************");
            #endif
            
            #ifdef LIVE
            /*
            myservo1.writeMicroseconds(1200);
            myservo2.writeMicroseconds(1200);
            myservo3.writeMicroseconds(1200);
            myservo4.writeMicroseconds(1200);
            */
            myservo1.writeMicroseconds(Power[0]);
            myservo2.writeMicroseconds(Power[1]);
            myservo3.writeMicroseconds(Power[2]);
            myservo4.writeMicroseconds(Power[3]);
            
            #endif

            //LastPI = now; //update time for the PI controller
      }
          
      delay(DISPLAY_INTERVAL);
}
void LockDrone(){
  for(int n=1200; n>=1000; n-=10){
      myservo1.writeMicroseconds(n);
      myservo2.writeMicroseconds(n);
      myservo3.writeMicroseconds(n);
      myservo4.writeMicroseconds(n);
      delay(100);
      if(n<=1000){
        delay(100000);
      }
   }
  while(true){
    delay(10000);
  }
}

