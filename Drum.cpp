#include "drum.h"
#include "math.h"

Drum::Drum(int AnalogReadPin=A4, int NoteAddress=SNARE_ADDRESS) {
    AnalogRead=0;
    HitStartTime=0;
    TimeSinceHit=0;  
    HighestRead=0;
    AnalogInputNumber=AnalogReadPin;
    MidiAddress=NoteAddress;
    State=IDLE;
}

//new features:Added setting up sensitivity, generating a logarithmic LUT, using the LUT inside ADCToVelocity, add HighestRead
//i will be passing the array through a pointer, not sure if this will work

void SetSensitivity(unsigned char SensitivityIn){//Sensitivity takes values from 1-10, 1 is the least sensitive
  
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
  
  int Velocity;
  AnalogRead = analogRead(AnalogInputNumber);
  
  switch(State){
  case IDLE:
    if(AnalogRead>THRESHOLD){
      HitStartTime=millis();
      State=WAIT_FOR_MAX;
    }
    break;
  case WAIT_FOR_MAX:
    TimeSinceHit=millis()-HitStartTime;
    if(AnalogRead>HighestRead){
      HighestRead=AnalogRead;
    }
    if(TimeSinceHit >= NOTE_BUFFER_TIME){
      State=SEND_NOTE_ON;
    }
    break;
  case SEND_NOTE_ON:
    //Serial.println(AnalogRead); //debug
    Velocity=ADCToVelocity(HighestRead);
    SendMidi(NOTE_ON, MidiAddress, Velocity); 
    State=BLOCK;
    break;
  case BLOCK:
    TimeSinceHit=millis()-HitStartTime;
    if(TimeSinceHit>=NOTE_LENGTH){
      State=SEND_NOTE_OFF;
    }
    break;
  case SEND_NOTE_OFF:
    SendMidi(NOTE_OFF, MidiAddress, 0);
    HitStartTime=0; 
    HighestRead=0;
    State=IDLE;
    break;
  default:
    State=IDLE;
    break;
  }
}
