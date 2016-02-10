#include "RadioCoder.h"

const int buttonPin = 2;     // the number of the pushbutton pin
int buttonState = 0;
int SentArray[5] = {0,0,0,0,0};
int sensorValue = 0;

RadioCoder radio; //make a new radio instance

void setup(){
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
}
void loop(){
  //Read the status of the killswitch

  // read the state of the pushbutton value, if the killswitch has been triggered, lock the system down
  if(digitalRead(buttonPin)){
    SentArray[0] = 1;
    while(true){
      radio.EncodeSent(SentArray);
    }
  }

  // read the input on analog pin 0 and sent it to the drone
  SentArray[1] = map(analogRead(A0),0,1023,0,100);
  radio.EncodeSent(SentArray);
}

