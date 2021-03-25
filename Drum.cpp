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

void SetSensitivity(unsigned char SensitivityIn, ResponseType Type){//Sensitivity takes values from 1-10, 1 is the least sensitive
  
  unsigned int ADC_IN;
  float Sensitivity;
  unsigned int ADCCalcValue;
  switch(Type){
    case logarithmic:
      Sensitivity=0.111*SensitivityIn+0.888;//scale sensitivity for current response type
      for(ADC_IN=0 ; ADC_IN<1024 ; ADC_IN++){  
        if( ADC_IN <= THRESHOLD ){
          ADCToVelConv[ADC_IN]=0;
        }
        else{
          //all the constants below are used so that the logarithm for sensitivity=1 crosses the ADC_IN axis close to ADC_IN=20 and vel=127 for ADC_IN~1000
          ADCToVelConv[ADC_IN]= 32.277 * Sensitivity * log( (double)(ADC_IN+20-THRESHOLD))-96.692*Sensitivity; 
          if(ADCToVelConv[ADC_IN] > MAX_VELOCITY){
            ADCToVelConv[ADC_IN] = MAX_VELOCITY;
          }
        }
      }
      break;
    case linear:
      Sensitivity=0.333*SensitivityIn+0.666;//scale sensitivity for current response type
      for(ADC_IN=0 ; ADC_IN<1024 ; ADC_IN++){
        if( ADC_IN <= THRESHOLD ){
          ADCToVelConv[ADC_IN]=0;
        }
        else{
          ADCCalcValue = Sensitivity*MAX_VELOCITY*(ADC_IN-THRESHOLD)/(1023-THRESHOLD);
          if(ADCCalcValue > MAX_VELOCITY){
            ADCToVelConv[ADC_IN] = MAX_VELOCITY;
          }
          else{
            ADCToVelConv[ADC_IN] = ADCCalcValue;
          }
        }
      }
      break;
    default:
      break;
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
