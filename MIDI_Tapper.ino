#include "Drum.h"

Drum MySnare(A5,SNARE_ADDRESS);
Drum MyHihat(A4,HIHAT_ADDRESS);

void setup() {
  Serial.begin(115200);
  MySnare.SetSensitivity(1);
}

void loop() {    
    MySnare.CheckHits();
}
