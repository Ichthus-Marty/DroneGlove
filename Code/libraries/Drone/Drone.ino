// If you need any help feel free to PM me (simonfrfr) or email me
// at newtonlabs@bellsouth.net
String readString;
#include <Servo.h>
#include "Drone.h"

#define Engine_1_pin 3
#define Engine_2_pin 5
#define Engine_3_pin 6
#define Engine_4_pin 9

Drone XD(true, Engine_1_pin, Engine_2_pin, Engine_3_pin, Engine_4_pin);

/*
Servo myservo1;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo
Servo myservo3;  // create servo object to control a servo
Servo myservo4;  // create servo object to control a servo
*/

void setup() {
  Serial.begin(115200);
  Serial.println("servo-test"); // so I can keep track of what is loaded
  /*
  myservo1.attach(3);
  myservo2.attach(5);
  myservo3.attach(6);
  myservo4.attach(9);
  */
}


void loop(){
      for(int n=100;n<200;n+=1){
        Serial.println("Microseconds:");
        Serial.println(n);

        XD.WriteAllEngines(n);
        /*
        myservo1.writeMicroseconds(n); // writes to ports
        myservo2.writeMicroseconds(n); // writes to ports
        myservo3.writeMicroseconds(n); // writes to ports
        myservo4.writeMicroseconds(n); // writes to ports
        */
        
        delay(2000);
      } 
}
