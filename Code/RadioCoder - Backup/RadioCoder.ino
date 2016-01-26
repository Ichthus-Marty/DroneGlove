#include "RadioCoder.h"
#include "ArrayParseExample.h"

RadioCoder Radio;

int TestVar[] = {0,0,0,0,0};
int* p = TestVar;
int Inst[5] = {0,100,5,20,90};

void setup() {
  Serial.begin(9600);
  
  String ToDecode = "0&100&5&20&90&;";
}

void loop(){
  
  p[1] = 989;
  delay(2000);
  
  //Radio.EncodeSent(Inst);

  Serial.println("Before ReceiveDecode value of TestVar:");
  for(int i=0;i<5;i++){
    Serial.println(p[i]);
  }
  
  if(Radio.ReceiveDecode(&p)){
     /*sizeof test
     Serial.println("Size of TestVar:");
     Serial.println(sizeof(TestVar));
     */
     
     Serial.println("After ReceiveDecode value of TestVar:");
     for(int i=0;i<5;i++){
      //receivedCommands
      Serial.println(p[i]);
        //Serial.println(TestVar[i]);
    }
    
    
  }

  Serial.println("-------------------");
  
}
