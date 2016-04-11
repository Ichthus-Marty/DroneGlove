/*
 SENT FINGER DATA TEST

 Receives from the hardware serial, sends to software serial.
 Receives from software serial, sends to hardware serial.

 The circuit:
 * RX is digital pin 10 (connect to TX of other device)
 * TX is digital pin 11 (connect to RX of other device)

 */

// COM16 = Writer_GLOVE
// COM14 = Writer
// COM15 = Reader
int Panic_stop = 0;

//#define Writer
#define Reader

#include <SoftwareSerial.h>

#ifdef Writer
  byte analogPins[] = { 
  A0, A1, A2, A3, A4, A5, A6, A7 };
#endif

#ifdef Reader
  SoftwareSerial mySerial(9, 11); // RX, TX  
#endif


void setup() {
  
  Serial.begin(9600, SERIAL_8N1);
  
  #ifdef Reader
    mySerial.begin(9600);
  #endif

  #ifdef Writer
  for(int i=0;i<8;i++){
    pinMode(analogPins[i], INPUT_PULLUP);  
  }
  #endif
}

void loop() { // run over and over
  //########################################################
  //Writer code
  //########################################################
  #ifdef Writer
     for(int i=0;i<8;i++){
      //Serial.write(analogRead(analogPins[i]));
      //delay(500); 
      if(analogRead(analogPins[A0]) > 340 && analogRead(analogPins[A7]) > 200){
        
        Panic_stop = 1;
    };
    
    Serial.println(Panic_stop);
      }
    delay(2000);
  #endif

  //########################################################
  //Reader code
  //########################################################
  #ifdef Reader
    if (mySerial.available()) {
    Serial.print(mySerial.read());
    Serial.print("********************************");
  }
  #endif
}
