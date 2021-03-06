#include "drum.h"



Drum::Drum(int AnalogReadPin=A4, int NoteAddress=SNARE_ADDRESS) {
    AnalogRead=0;
    NextAnalogRead=0;
    Velocity=0;
    ActiveHit=0;
    HitStartTime=0;
    TimeSinceHit=0;  
    AnalogInputNumber=AnalogReadPin;
    MidiAddress=NoteAddress;
}


void Drum::SendMidi(int MidiCommand, int NoteAddress, int NoteVelocity ) {
  Serial.write(MidiCommand);
  Serial.write(NoteAddress);
  Serial.write(NoteVelocity);
}


int Drum::ADCToVelocity(int AnalogRead){
  
  int Velocity;

  Velocity=(AnalogRead*(unsigned long)127)/(unsigned long)1023;
  return Velocity;
}


void Drum::CheckHits(void){
  
  AnalogRead = analogRead(AnalogInputNumber);
  
  if ( AnalogRead>THRESHOLD) { 
    ActiveHit=1;
  }
  else{}
  
  if( (ActiveHit==1) && (HitStartTime==0) ){
    HitStartTime=millis();
    while(millis()-HitStartTime <= NOTE_BUFFER_TIME){//change to not stay in while
      NextAnalogRead=analogRead(AnalogInputNumber);
      if(NextAnalogRead>AnalogRead){
        AnalogRead=NextAnalogRead;
      }
    }
    //Serial.println(AnalogRead);
    Velocity=ADCToVelocity(AnalogRead);
    SendMidi(NOTE_ON, MidiAddress, Velocity); 
  }
  else if( millis()-HitStartTime >= NOTE_LENGTH ){
    SendMidi(NOTE_OFF, MidiAddress, 0);
    HitStartTime=0;
    ActiveHit=0;
    Velocity=0;
  }  
}
