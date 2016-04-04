#include "RadioCoder.h"
#include <Servo.h> 

// COM14 = sender
// COM15 = receiver

#define SenderActive
//#define ReaderActive

//Servo myservo;
RadioCoder Radio;

int TestVar[] = {0,0,0,0,0};
int* p = TestVar;
int Inst[5] = {0,0,0,0,0};

void setup() {
  Serial.begin(115200);

  pinMode(8,OUTPUT);
  digitalWrite(8, HIGH);
  //Serial.setTimeout(100);
}

void loop(){
  //########################################################
  //Sender code
  //########################################################
  #ifdef SenderActive
  
  delay(10);
  /*
  Inst[0] = random(2);
  Inst[1] = random(101);
  Inst[2] = random(-90, 91);
  Inst[3] = random(-90, 91);
  Inst[4] = random(-90, 91);
  */

  Inst[0] = 0;
  Inst[1] = 65;
  Inst[2] = 0;
  Inst[3] = 0;
  Inst[4] = 0;
  
  Radio.EncodeSent(Inst);

  #endif
  
  //########################################################
  //Receiver code
  //########################################################
  #ifdef ReaderActive
  
  
  if(Radio.ReceiveDecode(&p)){
     Serial.println("Received values:");
     for(int i=0;i<5;i++){
        Serial.println(p[i]);
    }  
  Serial.println("-------------------");

  }
  
 else{
  //No value has been received
 }
  #endif

  //int* p = TestVar;
}
