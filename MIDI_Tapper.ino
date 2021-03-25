#include "Drum.h"

Drum MySnare(A5,SNARE_ADDRESS);
Drum MyHihat(A4,HIHAT_ADDRESS);

void setup() {
  Serial.begin(115200);
  
}

void loop() {    
    SetSensitivity(1,logarithmic); //SetSensitivity('value from 1-10', 'linear'/'logarithmic')
    while(1){
      MySnare.CheckHits();
    }
    
} 
