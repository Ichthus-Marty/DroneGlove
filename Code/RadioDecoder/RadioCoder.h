#ifndef RadioCoder_h
#define sonarRadioCoder_h

#include "Arduino.h"

class RadioCoder
{
  public:
    RadioCoder();
    int ReceiveDecode(int receivedCommands[5]);
    void EncodeSent(int sentCommands[5]);
    int receivedCommands[5];
  private:
	  int prevSepatorLocation;
	  int CurrentSeperatorLocation;
	  int radioReceivedCommands[5];
	  int commandCount;
	  String radioSentCommand;
	  String radioReceiveBuffer;
};

#endif
