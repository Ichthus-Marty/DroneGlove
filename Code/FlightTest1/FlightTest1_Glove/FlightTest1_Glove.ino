#include "RadioCoder.h"

int SentArray[5] = {0,10,0,0,0};

RadioCoder radio; //make a new radio instance

int PC_Command;

void setup(){
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  Serial.begin(115200);
  
}
void loop(){
  /*
  delay(50);
  Serial.print("0&210&200&200&200&;");
  */
  
  radio.EncodeSent(SentArray);
  
}

