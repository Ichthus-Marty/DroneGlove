#include <Servo.h>

Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo myservo4;

void setup() {
  Serial.begin(9600);
  delay(5000);
  
  myservo1.attach(3);
  myservo2.attach(5); 
  delay(100);
  myservo3.attach(6);
  myservo4.attach(9);
  delay(1000);
  
  myservo1.writeMicroseconds(1000);
  myservo2.writeMicroseconds(1000);
  delay(100);
  myservo3.writeMicroseconds(1000);
  myservo4.writeMicroseconds(1000);
  delay(2000);
}

void loop() {
   myservo1.writeMicroseconds(1200);
   myservo2.writeMicroseconds(1200);
   myservo3.writeMicroseconds(1200);
   myservo4.writeMicroseconds(1200);
   Serial.println(1200);

   if(millis()>18000){
    for(int n=1200; n>=1000; n-=10){
      Serial.println(n);
      myservo1.writeMicroseconds(n);
      myservo2.writeMicroseconds(n);
      myservo3.writeMicroseconds(n);
      myservo4.writeMicroseconds(n);
      delay(100);
      if(n<=1000){
        delay(100000);
      }
    }
    
   }
}
