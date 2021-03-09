#include "Drum.h"

void setup() {
  Serial.begin(115200);
}

Drum MySnare(A5,SNARE_ADDRESS);
Drum MyHihat(A4,HIHAT_ADDRESS);

void loop() {    
    MySnare.CheckHits();
    MyHihat.CheckHits();
}
