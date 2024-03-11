#ifndef Trafikljus_h
#define Trafikljus_h

#include "Arduino.h"

class Trafikljus{
  public:
    Trafikljus(int pinRed, int pinYellow, int pinGreen);
    void setGreen();
    void setYellow();
    void setRed();
    void setRedYellow();
    void set(int i);

  private:
    int RED;
    int YELLOW;
    int GREEN;
  
};
#endif
