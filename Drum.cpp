#include "drum.h"

Drum::Drum(int AnalogReadPin=A4, int NoteAddress=SNARE_ADDRESS) {
    AnalogRead=0;
    HitStartTime=0;
    TimeSinceHit=0;  
    AnalogInputNumber=AnalogReadPin;
    MidiAddress=NoteAddress;
    NextState=IDLE;
}

void Drum::SendMidi(int MidiCommand, int NoteAddress, int NoteVelocity ) {
  Serial.write(MidiCommand);
  Serial.write(NoteAddress);
  Serial.write(NoteVelocity);
}

int Drum::ADCToVelocity(int AnalogReadIn){
  
  int Velocity;
  
  Velocity=(AnalogReadIn*(unsigned long)127)/(unsigned long)1023;
  return Velocity;
}


void Drum::CheckHits(void){
  
  int Velocity, NextAnalogRead;
  AnalogRead = analogRead(AnalogInputNumber);
  

  switch(CurrentState){
  case IDLE:
    if(AnalogRead>THRESHOLD){
      HitStartTime=millis();
      NextState=SEND_NOTE_ON;
    }
    break;
  case WAIT_FOR_MAX:
    TimeSinceHit=millis()-HitStartTime;
    NextAnalogRead=analogRead(AnalogInputNumber);
    if(NextAnalogRead>AnalogRead){
      AnalogRead=NextAnalogRead;
    }
    if(TimeSinceHit >= NOTE_BUFFER_TIME){
      NextState=SEND_NOTE_ON;
    }
    break;
  case SEND_NOTE_ON:
    //Serial.println(AnalogRead); //debug
    Velocity=ADCToVelocity(AnalogRead);
    SendMidi(NOTE_ON, MidiAddress, Velocity); 
    NextState=BLOCK;
    break;
  case BLOCK:
    TimeSinceHit=millis()-HitStartTime;
    if(TimeSinceHit>=NOTE_LENGTH){
      NextState=SEND_NOTE_OFF;
    }
    break;
  case SEND_NOTE_OFF:
    SendMidi(NOTE_OFF, MidiAddress, 0);
    HitStartTime=0; 
    NextState=IDLE;
    break;
  default:
    NextState=IDLE;
    break;
  }
}
