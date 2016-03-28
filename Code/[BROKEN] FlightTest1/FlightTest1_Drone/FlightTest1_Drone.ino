#include "Wire.h"
#include "I2Cdev.h"
#include "MPU9150_9Axis_MotionApps41.h"
#include "sonar.h"
#include "RadioCoder.h"
#include "Drone.h"
#include <Servo.h>

#define Engine_1_pin 3
#define Engine_2_pin 5
#define Engine_3_pin 6
#define Engine_4_pin 9
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)

Drone drone(true, Engine_1_pin, Engine_2_pin, Engine_3_pin, Engine_4_pin);
MPU9150 mpu;
RadioCoder radio;
Sonar sonarHeight(7);

int Commands[] = {0,0,0,0,0};
int* pCommands = Commands;
bool blinkState = false;
int TimeNow;
int LastCommandMilli;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

void dmpDataReady() {
    mpuInterrupt = true;
}

float AVRG = 0;
int RUNS = 0;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
   float AVRG = 0;
   int RUNS = 0;
   
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    Serial.begin(115200);
 
        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        //Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = MPU_Setup();

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        if(radio.ReceiveDecode(&pCommands)){
          
          if(Commands[0]==1){ //The glove did a lockdown of the system, deactivate the drone and let it fall down
            lockDrone();
          }

        //Build-in selfdestrucion. Lock the drone when no commands were received for the last 60 milliseconds (glove sends every 20 milliseconds)  
        TimeNow = millis();
        
        if(TimeNow - LastCommandMilli >= 60){
          lockDrone();
        }

        LastCommandMilli = TimeNow;
        
        
        drone.WriteAllEngines(Commands[1]);
        }
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
    }
    // otherwise, check for DMP data ready interrupt (this should happen frequently)
     else if (mpuIntStatus & 0x01) {

        
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize){ 
          fifoCount = mpu.getFIFOCount();
        };
        
        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

  
        // display Euler angles in degrees
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        Serial.print("ypr\t");
        Serial.print(ypr[0] * 180/M_PI);
        Serial.print("\t");
        Serial.print(ypr[1] * 180/M_PI);
        Serial.print("\t");
        Serial.println(ypr[2] * 180/M_PI);

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
    }
}
bool MPU_Setup(){
    // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3v or Ardunio
    // Pro Mini running at 3.3v, cannot handle this baud rate reliably due to
    // the baud timing being too misaligned with processor ticks. You must use
    // 38400 or slower in these cases, or use some kind of external separate
    // crystal solution for the UART timer.

    // initialize device
    mpu.initialize();

    
    if(!mpu.testConnection()){
      return false;
    }

    devStatus = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0){
        mpu.setDMPEnabled(true);
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        return true;
    }
    else{
      return false;
    }
}
void lockDrone(){ //function to lockdown the drone
  int LockPower[4] = {0,0,0,0};
  drone.WriteAllEngines(*LockPower);
  while(true){
    delay(10000);
   }
}   
