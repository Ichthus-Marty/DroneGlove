#include "RadioCoder.h"

RadioCoder Radio;

void setup() {
  Serial.begin(9600);
  int Inst[5] = {0,100,5,20,90};
  String ToDecode = "0&100&5&20&90&;";
  int AlphaMale[5];
}

void loop(){
  delay(2000);
  
  //int Inst[5] = {0,100,5,20,90};
  //Radio.EncodeSent(Inst);
  
  int TestVar[5] = {0,0,0,0,0};

  
  Serial.println("Before ReceiveDecode value of TestVar:");
  for(int i=0;i<5;i++){
    Serial.println(TestVar[i]);
  }
  
  if(Radio.ReceiveDecode(TestVar)){
     /*sizeof test
     Serial.println("Size of TestVar:");
     Serial.println(sizeof(TestVar));
     */
     
     Serial.println("After ReceiveDecode value of TestVar:");
     for(int i=0;i<5;i++){
        Serial.println(TestVar[i]);
    }
    
  }

  Serial.println("-------------------");
  
}
