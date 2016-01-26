#include "RadioCoder.h"
#include <Servo.h> 

// COM14 = sender
// COM15 = receiver

//#define SenderActive
#define ReaderActive

//Servo myservo;
RadioCoder Radio;

int TestVar[] = {0,0,0,0,0};
int* p = TestVar;
int Inst[5] = {0,0,0,0,0};
//String ToDecode = "0&100&5&20&90&;";

void setup() {
  Serial.begin(115200);

  pinMode(8,OUTPUT);
  digitalWrite(8, HIGH);
  Serial.setTimeout(100);
}

void loop(){
  //########################################################
  //Sender code
  //########################################################
  #ifdef SenderActive
  
  delay(200);

  /*
  int sensorValue = analogRead(A0);
  Inst[1] = map(sensorValue,0,1020,0,100);
  */
  Inst[0] = random(2);
  Inst[1] = random(101);
  Inst[2] = random(-90, 91);
  Inst[3] = random(-90, 91);
  Inst[4] = random(-90, 91);
  
  Radio.EncodeSent(Inst);
  /*
  Serial.println("Inst:");
  for(int i=0;i<5;i++){
    Serial.println(Inst[i]);
  }
  */
  
  #endif
  
  //########################################################
  //Receiver code
  //########################################################
  #ifdef ReaderActive
  
  //delay(1000);
  
  if(Radio.ReceiveDecode(&p)){
     Serial.println("Received values:");
     for(int i=0;i<5;i++){
        Serial.println(p[i]);
    }  
  Serial.println("-------------------");

  /*
    int angle = map(p[1], 0, 100, 0, 180);
    myservo.write(angle);
   */
  }
  
 else{
  /*
  Serial.println("No value received");
  Serial.println("-------------------");
  */
 }
  #endif

  //int* p = TestVar;
}
