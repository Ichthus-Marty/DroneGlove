#ifndef Drone_h
#define Drone_h

#include "Arduino.h"

class Drone
{
  public:
    Drone(bool Attach, int pinEngine1, int pinEngine2, int pinEngine3, int pinEngine4);
    void WriteSingleEngine(int EngineInt, int Throttle);
    void WriteSepEngines(int ThrottleArray[4]);
    void WriteAllEngines(int Throttle);
    void Write_X_Engines(int Throttle);
    void Write_Y_Engines(int Throttle);
    void ConfigEngines(int lowValue, int highValue);
  private:
    int _pinEngine1;
    int _pinEngine2;
    int _pinEngine3;
    int _pinEngine4;
};

#endif
