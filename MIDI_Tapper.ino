#include "Drum.h"

void setup() {
  Serial.begin(115200);
}

Drum MySnare(A4,SNARE_ADDRESS);
Drum MyHihat(A5,HIHAT_ADDRESS);

void loop() {    
    MySnare.CheckHits();
    MyHihat.CheckHits();
}
