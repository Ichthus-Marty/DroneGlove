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
//DEFINING LIVE (by uncommenting the LIVE variable below) WILL ARM AND ACTIVATE THE DRONE WHEN POWERED UP!!
/**************WARNING**************/
 
//#define LIVE

//#define DEBUG_POWER       //Uncomment to read the output to the engines
//#define DEBUG             //Uncomment to read the process of the script
//#define Gyro_Cal          //Uncomment to read the raw output of the gyro's and the sonar. Use this to determine the sensor offsets. 

RTIMU *imu;                                           // the IMU object
RTFusionRTQF fusion;                                  // the fusion object
RTIMUSettings settings;                               // the settings object
Sonar SonarHeight(7);                                 // The sonar object
RadioCoder Radio;                                     // Radio object for wireless communication

//Create the servo objects
Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;

//Sensor offsets. Define Gyro_Cal to read the raw data of the gyro's and the sonar.
#define rollOffset  1.60
#define pitchOffset -3.48
#define sonarOffset 3.0

//Time to wait before the engines power up, needed to warm up the gyro
#define warmUpDelay 15000

//  DISPLAY_INTERVAL sets the rate at which results are displayed
#define DISPLAY_INTERVAL  5                         // interval between pose displays

//  SERIAL_PORT_SPEED defines the speed to use for the debug serial port
#define  SERIAL_PORT_SPEED  115200

//  Variables for the IMU
unsigned long lastDisplay;
unsigned long lastRate;
bool ErrorCaught = false;
int sampleCount;
float pitchPrev = 0.0;
float rollPrev = 0.0;
float Error_RollPrev = 0.0;

// Sonar variable
int Height;

// Radio variables
int Rec_Instructs[] = {0,0,0,0,0};      //Array to hold received intstuctions
int* pRI = Rec_Instructs;               //Pointer to array above
int RadioPower;                         //Holds the pRI[1] value, used multiple times in the PI
unsigned long timeLastCommand = 0;
bool runFlag = false;
int RadioLock;
float RadioRoll;
float RadioPitch;
//float RadioYaw;

//PI variabales
float Cp = 0.0013; //Constant for the P
float Ci = 0.0;
float Error_Pitch = 0.0;
float Error_Roll =0.0 ;
float errSum_Pitch =0.0 ;
float errSum_Roll = 0.0;
float LastPI = 0;

//Drone variables
float Power[4] = {0.0,0.0,0.0,0.0}; // Array for the data output for each engine.

void setup()
{
    int errcode;

    //Make pin 8 heigh to active the radio
    pinMode(8,OUTPUT);
    digitalWrite(8, HIGH);
  
    Serial.begin(SERIAL_PORT_SPEED);
    Serial.setTimeout(100);
    #ifdef DEBUG
      Serial.println("SETTING UP...");
    #endif
    Wire.begin();
    imu = RTIMU::createIMU(&settings);                        // create the imu object

    #ifdef DEBUG
      Serial.println("Created IMU");
    #endif
  
    imu->IMUName();

    #ifdef DEBUG
      Serial.println("got IMUName");
    #endif
    
    if ((errcode = imu->IMUInit()) < 0) {
        ErrorCaught = true;
        #ifdef DEBUG
          Serial.print("Failed to init IMU: "); Serial.println(errcode);
        #endif     
    }
  
    imu->getCalibrationValid();

    #ifdef DEBUG
        Serial.println("IMU calibration validated");
    #endif  

    lastDisplay = lastRate = millis();
    sampleCount = 0;

    // Slerp power controls the fusion and can be between 0 and 1
    // 0 means that only gyros are used, 1 means that only accels/compass are used
    // In-between gives the fusion mix.
    
    fusion.setSlerpPower(0.02);

    #ifdef DEBUG
       Serial.println("fusion slerpPower set");
    #endif  
    
    // use of sensors in the fusion algorithm can be controlled here
    // change any of these to false to disable that sensor
    
    fusion.setGyroEnable(true);
    fusion.setAccelEnable(true);
    fusion.setCompassEnable(true);

    #ifdef DEBUG
       Serial.println("fusion sensors enabled");
    #endif 

    /*
    Serial.println("Units:");
    Serial.println("Pose: Roll, Pitch, Magnetic Heading (measured in degrees)");
    Serial.println("Gyro: Rate of Roll, Pitch, and Yaw (measured in degrees per second)");
    Serial.println("Accl: Acceleration in X, Y, and Z (measured in g's, with 1 g equal to 9.8 meters per second squared)");
    Serial.println("Time: Timestamp (measured in milliseconds from when the MPU-9150 was initiated)");
    Serial.println("----------------------------");
    */

    //Attach the engines, use a delay to give them some time to attach
    myservo1.attach(3);
    myservo2.attach(5); 
    delay(100);
    myservo3.attach(6);
    myservo4.attach(9);
    delay(1000);

    #ifdef DEBUG
       Serial.println("Servos attached");
    #endif

    //Arm the engines
    myservo1.writeMicroseconds(0);
    myservo2.writeMicroseconds(0);
    delay(100);
    myservo3.writeMicroseconds(0);
    myservo4.writeMicroseconds(0);
    delay(5000);
    #ifdef DEBUG
      Serial.println("Engines armed");
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

    while(imu->IMURead()){// get the latest data if ready yet
      now = millis();


      //Read the radio variables and validate them (the radio sometimes generates spikes and unexpected output)
      if(Radio.ReceiveDecode(&pRI)){
            timeLastCommand = now;
            
            RadioLock = pRI[0];
            if(RadioLock != 0 && RadioLock != 1){
              continue;
            }
            if(RadioLock==1){
              LockDrone();
            }
            RadioPower = pRI[1]*10;
            if(RadioPower< 0 || RadioPower > 1000){
              continue;
            }
            /*
            RadioYaw = pRI[2];
            if(RadioPitch< -??VALUE?? || RadioPitch > ??VALUE??){
              continue;
            }
            */
            RadioPitch = pRI[3];
            if(RadioPitch < -90 || RadioPitch > 90){
              continue;
            }
            
            RadioRoll = pRI[4];
            if(RadioRoll < -90 || RadioRoll > 90){
              continue;
            }   
         }

         //Lock the drone if we haven't heard from the radio for 2 seconds
         if(now - timeLastCommand >= 2000 && runFlag == true){
            LockDrone();
         }   
              
        // this flushes remaining data in case we are falling behind. 
        // Keeps removing data from FIFO buffer until it's empty, then returns from the loop and reinitializes the loopcount variable
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
          if(roll>90.0 || roll<-90.0){ //Safety issue
            return;

           #ifdef DEBUG
             Serial.println("Too much roll");
           #endif
          }
          
          float pitch = pose.x()*d - pitchOffset;         // nose down is negative
          /*
          if(pitch>90.0 || pitch<-90.0){
            continue;
          }
          if(abs(abs(pitch)-abs(pitchPrev))>=10 && runFlag){
            return;
          }
          */
          pitchPrev = pitch;
          rollPrev = roll;
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

          //Used when booted up to give the gyro some time to warm up
           if(now<warmUpDelay){
              #ifdef DEBUG
                Serial.println("warmUpDelay");
              #endif
              return;
           }
           
          #ifdef Gyro_Cal
          Serial.print("YAW:  ");
          Serial.println(yaw);
          Serial.print("PITCH:  ");
          Serial.println(pose.x()*d);
          Serial.print("ROLL:  ");
          Serial.println(pose.y()*d*-1);
          Serial.print("HEIGHT:  ");
          Serial.println(SonarHeight.GetHeight());
          Serial.println("---------------------------------------------------------------");
          #endif

          if(!runFlag){
            runFlag = true;
            LastPI =  now;
          }  
           
           

            /*NOTE TO SELF:
             * 
             * ONLY USE THE PI OF THE PID CONTROLLER. 
             * D WILL MAKE DEBUGGING A PAIN IN THE *SS
             * 
             */

            
            //******************************************PID********************************************************
            Power[0] = 0;
            Power[1] = 0;
            Power[2] = 0;
            Power[3] = 0;
            
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
            Error_Roll = RadioRoll - roll;
            
            if(Error_Roll > 90 || Error_Roll<-90){
              #ifdef DEBUG
                Serial.println("Error_Roll too big");
              #endif
              return;
            }

            /*
            if(abs(abs(Error_Roll)-abs(Error_RollPrev))>= 30 && runFlag){
              #ifdef DEBUG
                Serial.println("Delta Error_Roll too big");
              #endif
              return;
            }
            */
            Error_RollPrev = Error_Roll;

            #ifdef DEBUG
            Serial.print("Error_Roll: ");
            Serial.println(Error_Roll);
            Serial.print("Roll: ");
            Serial.println(roll);
            #endif
            
            //Calculate roll I
            errSum_Roll += (Error_Roll * (now-LastPI));
            #ifdef DEBUG
            Serial.print("now-LastPI: ");
            Serial.println(now-LastPI);
            Serial.print("errSum_Roll:  ");
            Serial.println(errSum_Roll);
            #endif
            

            //Update Power array
            Power[0] += (Cp*Error_Roll) + (Ci*errSum_Roll);
            Power[1] -= (Cp*Error_Roll) - (Ci*errSum_Roll);
            Power[2] -= (Cp*Error_Roll) - (Ci*errSum_Roll);
            Power[3] += (Cp*Error_Roll) + (Ci*errSum_Roll);
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
              Power[i] = 1000+RadioPower+(Power[i]*RadioPower);
              if(Power[i]>2000){
                #ifdef DEBUG
                  Serial.println("Power[i] too big");
                #endif
                Power[i] = 2000;
                //return;
              }
              if(Power[i]<1000){
                #ifdef DEBUG
                  Serial.println("Power[i] too small");
                #endif
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
            myservo1.writeMicroseconds(Power[0]);
            myservo2.writeMicroseconds(Power[1]);
            myservo3.writeMicroseconds(Power[2]);
            myservo4.writeMicroseconds(Power[3]);
            
            #endif

            LastPI = now; //update time for the PI controller
      }
          
      delay(DISPLAY_INTERVAL);
}
void LockDrone(){
  //Let the drone lock down
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
