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

int RadioCoder::ReceiveDecode(int** p){
//int RadioCoder::ReceiveDecode(int* p){
//int RadioCoder::ReceiveDecode(TestVar){
  /*
   * The format of the radiomessages
   * 
   * Format: lockStatus&Throttle&gX&gY&gZ;
   * 
   * lockStatus (true or false)
   * Throttle (between 0% and 100%)
   * gyroX (between -90 and +90)
   * gyroY (between -90 and +90)
   * gyroZ (between -90 and +90)
   * 
  */

  if(Serial.available() > 0){

	   //Extract the first string with commands (seperated by ';') from the Serial buffer and split it into the seperate commands 
	   radioReceiveBuffer = Serial.readStringUntil(';');

	   for(int possition =0; possition<=radioReceiveBuffer.length(); possition++){
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

    /*
    for(int i=0;i<5;i++){
      Serial.println(radioReceivedCommands[i]);
    }
    */
    
	  *p = radioReceivedCommands;
    //Serial.println("Size of receivedCommands:");
    //Serial.println(sizeof(receivedCommands));
    
    Serial.println("During loop value of receivedCommands");
    for(int i=0;i<5;i++){
      Serial.println(*p[i]);
    }
    
   
	  /*
	  receivedCommands -> Throttle = radioReceivedCommands[1];
	  receivedCommands -> gX = radioReceivedCommands[2];
	  receivedCommands -> gY = radioReceivedCommands[3];
	  receivedCommands -> gZ = radioReceivedCommands[4];
	  */

	  return 1;
  }
  else{
    return 0;
  }
};
void RadioCoder::EncodeSent(int sentCommands[5]){
  //Clear previous data in the variable
  radioSentCommand = "";
  
  for(int i=0;i<5;i++){
	  radioSentCommand += String((int)sentCommands[i], (unsigned char)DEC);
    radioSentCommand += "&";
  };
  radioSentCommand += ";";

  Serial.println(radioSentCommand);
};

