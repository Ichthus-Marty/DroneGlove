/*
   * The format of the radiomessages
   * 
   * Format: lockStatus&Throttle&gX&gY&gZ&;
   * 
   * lockStatus (true or false)
   * Throttle (between 0% and 100%)
   * gyroX (between -90 and +90)
   * gyroY (between -90 and +90)
   * gyroZ (between -90 and +90)
   * 
   * Total string length: 18 characters
  */

#include "Arduino.h"
#include "RadioCoder.h"

RadioCoder::RadioCoder(){
  String radioReceiveBuffer;
  
  int prevSepatorLocation = 0;
  int CurrentSeperatorLocation;
  int radioReceivedCommands[5];
  int commandCount =0;
  String radioSentCommand;
};

int RadioCoder::ReceiveDecode(int** ppCommandArray){
  /*
   * This funcion reads the serialbuffer of the Arduino and extracts the commands that the sender sent over the radioshield. It stores the received values in the array that p points to
   */
  
  if(Serial.available() > 0){
     //Extract the first string with commands (seperated by ';') from the Serial buffer and split it into the seperate commands 
     radioReceiveBuffer = Serial.readStringUntil(';');
     
     //Serial.println(radioReceiveBuffer);
     
    //Serial.println(radioReceiveBuffer);
    //See if the recieved string is 18 characters long as it's supposed to  
    if(radioReceiveBuffer.length()!= 18){
      return 0;
    }

   //Reset the variables needed to extract the commands
   commandCount = 0;
   prevSepatorLocation = 0;
    
   for(int possition = 0; possition<radioReceiveBuffer.length(); possition++){
    if(radioReceiveBuffer.substring(possition, possition+1) == "&"){

      //Needed to get rit of the '&' before the last 4 commands
      if(prevSepatorLocation>0){
        prevSepatorLocation = prevSepatorLocation+1;
      };
      
      radioReceivedCommands[commandCount] = radioReceiveBuffer.substring(prevSepatorLocation,possition).toInt();
  
      prevSepatorLocation = possition;
      commandCount++;
     };
   };

    //Substract 200 to the throttle, gX, gY and gZ values to make sure they consist of 3 characters, we needed it to verify the received command earlier
    for(int i=1;i<5;i++){
        radioReceivedCommands[i] -= 200;
    }
    
    *ppCommandArray = radioReceivedCommands;
  
    return 1;
  }
  else{
    return 0;
  }
};
void RadioCoder::EncodeSent(int sentCommands[5]){
  /*
   * This function takes the sentCommands array as input and transforms them into a string that the radioshield can send
   */
  
  //Clear previous data in the variable
  radioSentCommand = "";
  
  for(int i=0;i<5;i++){
    //Add 200 to the throttle, gX, gY and gZ values to make sure they consist of 3 characters, we need it to verify the received command later on
    if(i>0){
      radioSentCommand += String((int)(sentCommands[i]+200), (unsigned char)DEC);
    }
    else{
      radioSentCommand += String((int)sentCommands[i], (unsigned char)DEC);
    }
    radioSentCommand += "&";
  };
  
  radioSentCommand += ";";

  Serial.print(radioSentCommand);
};

