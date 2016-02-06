// If you need any help feel free to PM me (simonfrfr) or email me
// at newtonlabs@bellsouth.net
String readString;
#include <Servo.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;  // create servo object to control a servo
Servo myservo3;  // create servo object to control a servo
Servo myservo4;  // create servo object to control a servo

void setup() {
  Serial.begin(9600);
  Serial.println("servo-test"); // so I can keep track of what is loaded
  myservo1.attach(3);
  myservo2.attach(5);
  myservo3.attach(6);
  myservo4.attach(9);
}


void loop(){
      for(int n=0;n<2000;n+=100){
        Serial.println("Microseconds:");
        Serial.println(n);
        
        myservo1.writeMicroseconds(n); // writes to ports
        myservo2.writeMicroseconds(n); // writes to ports
        myservo3.writeMicroseconds(n); // writes to ports
        myservo4.writeMicroseconds(n); // writes to ports
        
        delay(1000);
      } 
}
