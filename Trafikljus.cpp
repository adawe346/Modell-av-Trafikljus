#include "Arduino.h"
#include "Trafikljus.h"

int RED;
int YELLOW;
int GREEN;

int state;


Trafikljus::Trafikljus(int pinRed, int pinYellow, int pinGreen){
  RED = pinRed;
  YELLOW = pinYellow;
  GREEN = pinGreen;

  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  
  setRed();
}

void Trafikljus::setGreen(){
  digitalWrite(GREEN, HIGH);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
  state = 2;
}

void Trafikljus::setYellow(){
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(RED, LOW);
  state = 1;
}

void Trafikljus::setRed(){
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, HIGH);
  state = 3;
}

void Trafikljus::setRedYellow(){
  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, HIGH);
  digitalWrite(RED, HIGH);
  state = 0;
}

void Trafikljus::set(int i){
  if(i==0)setRed();
  if(i==1)setYellow();
  if(i==2)setGreen();
}
