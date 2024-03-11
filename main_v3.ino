#include "Trafikljus.h"

int queue[] = {1,0};

Trafikljus t4 = Trafikljus(48,50,52); // Grupp 2 Trafikljus(Röd, Gul, Grön, ÖVERGÅNG GRÖN, ÖVERGÅNG RÖD)
Trafikljus t3 = Trafikljus(46,44,42); // Grupp 2
Trafikljus c2 = Trafikljus(26,31,27); // Grupp 2
Trafikljus t2 = Trafikljus(40,38,36); // Grupp 1
Trafikljus t1 = Trafikljus(34,32,30); // Grupp 1
Trafikljus c1 = Trafikljus(28,47,29); // Grupp 1

int k1 = 22; // Knapp Grupp 1
int k2 = 23; // Knapp Grupp 2

int cDelayTime = 1;
boolean ind = true;

const byte npulse   = 6;
const bool debug    = true;
const byte pin_pulse= A10;   //Skickar ut 258 pulser som sedan läses av pin_cap
const byte pin_cap  = A15;   //Läser värden från arduinon och kapacitatorn.
const byte pin_LED1 = 12;
const byte pin_LED2 = 11;

const int nmeas = 128;                //measurements to take
long int sumsum = 0;                  //running sum of 64 sums
long int skip = 0;                    //number of skipped sums
long int diff = 0;                    //difference between sum and avgsum
long int flash_period = 0;            //period (in ms)
long unsigned int prev_flash = 0;     //time stamp of previous flash



void setup(){
  pinMode(k1, INPUT);
  pinMode(k2, INPUT);


  if (debug) Serial.begin(9600); //De kalkulerade värdena skrivs ut i konsolen med hastigheten 9600 bits per second.
  //Pins:en(portarna/ingångarna) initieras.
  pinMode(pin_pulse, OUTPUT);
  digitalWrite(pin_pulse, LOW);
  pinMode(pin_cap, INPUT); 
  pinMode(pin_LED1, OUTPUT);
  digitalWrite(pin_LED1, LOW);
  pinMode(pin_LED2, OUTPUT);
  digitalWrite(pin_LED2, LOW);

  long tempTime = millis();
  cDelay(1);
  cDelayTime = millis() - tempTime;
  
  Serial.begin(9600);
}

void loop(){
  String st1 = "Kön är: ";
  String st2 = st1 + queue[0];
  String st3 = st2 + ",";
  String st4 = st3 + queue[1];
  Serial.println(st4);
  cDelay(1);
  runQueue();
}

void cDelay(int t){
  // Loop, ingen lång delay!!

  for(int i=0; i<t; i++){
    if(digitalRead(k1)==HIGH) addQueue(2); // Grupp 1 måste bli röd
    //if(digitalRead(k2)==HIGH) addQueue(1); // Grupp 2 måste bli röd

    if(ind){
      int minvalue=1023;
      int maxvalue=0;
   
      //perform measurement
      long unsigned int sum=0;
      
      //Koden mellan rad 80 och rad 160 står för metalldetektionen och är ursprungligen tagen från https://www.instructables.com/id/Simple-Arduino-Metal-Detector/ . Vi har dock modifierat koden mycket för att ändras till vårat ändamål. 
      for (int imeas=0; imeas<nmeas+2; imeas++){
        //Återställer kapacitatorn genom att ändra 'pin cap' från input till output och sedan input igen.
        pinMode(pin_cap,OUTPUT);
        digitalWrite(pin_cap,LOW);
        delayMicroseconds(20);
        pinMode(pin_cap,INPUT);
        //Skickar pulser ('npusle' antal pulser).
        for (int ipulse = 0; ipulse < npulse; ipulse++) {
          digitalWrite(pin_pulse,HIGH); //takes 3.5 microseconds
          delayMicroseconds(3);
          digitalWrite(pin_pulse,LOW);  //takes 3.5 microseconds
          delayMicroseconds(3);
        }
        //Läser laddningen på kapacitatorn
        int value = analogRead(pin_cap); //takes 13x8=104 microseconds
        minvalue = min(value,minvalue);
        maxvalue = max(value,maxvalue);
        sum+=value;
        //determine if LEDs should be on or off
        long unsigned int timestamp=millis();
        byte ledstat=0;
        if (timestamp<prev_flash+10){
          if (diff>137)ledstat=1;
          if (diff<137)ledstat=2;
        }
        if (timestamp>prev_flash+flash_period){
          if (diff>137)ledstat=1;
          if (diff<137)ledstat=2;
          prev_flash=timestamp;  
        }
        if (flash_period>1000)ledstat=0;
        //switch the LEDs to this setting
        if (ledstat==0){
          digitalWrite(pin_LED1,LOW);
          digitalWrite(pin_LED2,LOW);
        }
        if (ledstat==1){
          digitalWrite(pin_LED1,HIGH);
          digitalWrite(pin_LED2,LOW);
        }
        if (ledstat==2){
          digitalWrite(pin_LED1,LOW);
          digitalWrite(pin_LED2,HIGH);
          addQueue(2);
          diff = 137;
        }
      }                    //**
    
       
        
     
      //process
      long int avgsum=(sum/130);
      diff=avgsum;
      // one permille change = 2 ticks/s
      if (diff==0) flash_period=1000000;
      else flash_period=avgsum/(2*abs(diff)); 
      
      if (debug){
        Serial.print(nmeas);
        Serial.print(" ");
        Serial.print(minvalue);
        Serial.print(" ");
        Serial.print(maxvalue);
        Serial.print(" ");
        Serial.print(sum);
        Serial.print(" ");
        Serial.print(avgsum);
        Serial.print(" ");
        Serial.print(diff);
        Serial.print(" ");
        Serial.print(flash_period);
        Serial.print(" ");
        Serial.print(cDelayTime);
        Serial.println();
      }
    }
   
  //Finns i ursprungliga koden en } här vilken enligt mina beräkningar stänger den loopen som alltid körs i deras program. 
  //Alltså är det i vårt fall slutet på koden då vi inte behöver den loopen då vi använder cDealy. 

  delay(1);
  }
}

void runQueue(){
  switch(queue[0]){
   case 1:
      // Grupp 2 blir röd och grupp 1 blir grön
      ind = false;
      greenToRed(t3, t4, c2);
      cDelay(2000);
      redToGreen(t1, t2, c1);
      cDelay(15000);   //Huvudleden ska hållas grön ett tag innan sidoleden ska kunna bytas till grön. OBS! OM DET KÄNNS SOM ATT DEN INTE REAGERAR DIREKT SÅ SÄNK DETTA VÄRDE FÖR DEMOSTRATION.
      ind = true;
      
      Serial.println("Grupp 1 är grön");
      break;
    
    case 2:
      // Grupp 1 blir röd och grupp 2 blir grön
      ind = false;
      greenToRed(t1, t2, c1);
      cDelay(2000);
      redToGreen(t3, t4, c2);
      cDelay(15000);   //Huvudleden ska hållas grön ett tag innan sidoleden ska kunna bytas till grön. OBS! OM DET KÄNNS SOM ATT DEN INTE REAGERAR DIREKT SÅ SÄNK DETTA VÄRDE FÖR DEMOSTRATION. Huvud
      ind = true;

      addQueue(1); // Återgå till grupp 1
      
      Serial.println("Grupp 2 är grön");
      break;

    default:
      // 0 -> gör inget
      Serial.println("Kön är tom");
  }

  // Förskjut kön till vänster
  queue[0] = queue[1];
  queue[1] = 0;
}

void addQueue(int a){
  // Loopa genom alla positioner i kön
  if(queue[0]==a){}
  else{
    if(queue[0]==0){
        queue[0]=a;
    }
    if(queue[0]==a+1){
      if(queue[1]==a){
      } 
      else{
        if(queue[1]==0){
        queue[1]=a;
        }
      }
    }
  }
}

void redToGreen(Trafikljus a, Trafikljus b, Trafikljus c){
  a.setRedYellow();
  b.setRedYellow();
  c.setRedYellow();
  cDelay(3000);
  a.setGreen();
  b.setGreen();
  c.setGreen();
}

void greenToRed(Trafikljus a, Trafikljus b, Trafikljus c){
  a.setYellow();
  b.setYellow();
  cDelay(3000);
  a.setRed();
  b.setRed();
  c.setRed();
}
