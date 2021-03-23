#include "drum.h"
#include "math.h"

Drum::Drum(int AnalogReadPin=A4, int NoteAddress=SNARE_ADDRESS) {
    AnalogRead=0;
    HitStartTime=0;
    TimeSinceHit=0;  
    AnalogInputNumber=AnalogReadPin;
    MidiAddress=NoteAddress;
    CurrentState=IDLE;
}

//new features, not tested: Generating ADCtoVelocity conversion table, adding it to class elements
//i will be passing the array through a pointer, not sure if this will work
 
void Drum::SetSensitivity(unsigned char SensitivityIn){//Sensitivity takes values from 1-10, 1 is the least sensitive
  
  unsigned int ADC_IN;
  const float ScalingFactor=18.5;
  float Sensitivity=0.5*SensitivityIn+0.5;
  
  for(ADC_IN=0 ; ADC_IN<1023 ; ADC_IN++){  
    if( ADC_IN < THRESHOLD ){
      ADCToVelConv[ADC_IN]=0;
    }
    else{
      ADCToVelConv[ADC_IN]= ScalingFactor * log( (double)(ADC_IN-THRESHOLD) / (double)(Sensitivity) ); 
      if(ADCToVelConv[ADC_IN] > MAX_VELOCITY){
        ADCToVelConv[ADC_IN] = MAX_VELOCITY;
      }
    }
  }
}

void Drum::SendMidi(int MidiCommand, int NoteAddress, int NoteVelocity ) {
  Serial.write(MidiCommand);
  Serial.write(NoteAddress);
  Serial.write(NoteVelocity);
}

int Drum::ADCToVelocity(int AnalogReadIn){
  
  int Velocity;
  
  Velocity=ADCToVelConv[AnalogReadIn];
  return Velocity;
}

void Drum::CheckHits(void){
  
  int Velocity, NextAnalogRead;
  AnalogRead = analogRead(AnalogInputNumber);
  
  switch(CurrentState){
  case IDLE:
    if(AnalogRead>THRESHOLD){
      HitStartTime=millis();
      CurrentState=WAIT_FOR_MAX;
    }
    break;
  case WAIT_FOR_MAX:
    TimeSinceHit=millis()-HitStartTime;
    NextAnalogRead=analogRead(AnalogInputNumber);
    if(NextAnalogRead>AnalogRead){
      AnalogRead=NextAnalogRead;
    }
    if(TimeSinceHit >= NOTE_BUFFER_TIME){
      CurrentState=SEND_NOTE_ON;
    }
    break;
  case SEND_NOTE_ON:
    //Serial.println(AnalogRead); //debug
    Velocity=ADCToVelocity(AnalogRead);
    SendMidi(NOTE_ON, MidiAddress, Velocity); 
    CurrentState=BLOCK;
    break;
  case BLOCK:
    TimeSinceHit=millis()-HitStartTime;
    if(TimeSinceHit>=NOTE_LENGTH){
      CurrentState=SEND_NOTE_OFF;
    }
    break;
  case SEND_NOTE_OFF:
    SendMidi(NOTE_OFF, MidiAddress, 0);
    HitStartTime=0; 
    CurrentState=IDLE;
    break;
  default:
    CurrentState=IDLE;
    break;
  }
}
