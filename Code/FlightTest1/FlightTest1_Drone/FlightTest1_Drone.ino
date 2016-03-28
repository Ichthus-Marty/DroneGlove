#include <Servo.h>
#include "Wire.h"

#include "RadioCoder.h"
#include "Drone.h"


#define Engine_1_pin 3
#define Engine_2_pin 5
#define Engine_3_pin 6
#define Engine_4_pin 9

int now;
int lastReceived = 0;
int Commands[] = {0,0,0,0,0};
int* pCommands = Commands;

Drone drone(true, Engine_1_pin, Engine_2_pin, Engine_3_pin, Engine_4_pin);
RadioCoder radio;

void setup()
{
    pinMode(8, OUTPUT);
    digitalWrite(8, HIGH);
    Serial.begin(115200);
    Wire.begin();
    //Serial.println("Waiting order");
    while(!radio.ReceiveDecode(&pCommands));
    drone.WriteAllEngines(1000);
    delay(2000);
}

void loop(){
            now = millis();
                   
            if(radio.ReceiveDecode(&pCommands)){
              lastReceived = now;

              if(Commands[0]==1){ //The glove did a lockdown of the system, deactivate the drone and let it fall down
                lockDrone();
              }
            
            }

            
            if(now - lastReceived >= 300){
                lockDrone();
            }
            
            int Writerval = 1000 + Commands[1]*10;
            drone.WriteAllEngines(Writerval);
}
void lockDrone(){ //function to lockdown the drone
  int LockPower[4] = {0,0,0,0};
  drone.WriteAllEngines(*LockPower);

  Serial.println("LOCKED!");

}   
