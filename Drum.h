#ifndef DRUM_H
#define DRUM_H

#include "Arduino.h"

//MIDI addresses
#define SNARE_ADDRESS 26
#define HIHAT_ADDRESS 42
#define KICK_ADDRESS 24
#define FLOOR_ADDRESS 35
#define CRASH_ADDRESS 52

//Control
#define NOTE_BUFFER_TIME 1 //in miliseconds
#define NOTE_LENGTH 60 // in miliseconds
#define THRESHOLD 20   // in ADC value - 0-1023
#define MAX_VELOCITY 127

static unsigned char ADCToVelConv[1024]; 
enum ResponseType{logarithmic, linear};
enum NoteMode{noteOn, noteOff};
void SetSensitivity(unsigned char SensitivityIn, ResponseType Type); 

class Drum{
	
  public:   
    Drum(int AnalogInputNumber=A4, int MidiAddress=SNARE_ADDRESS);
    void CheckHits(void);
    void SendMidi(NoteMode MidiCommand, int NoteAddress, int NoteVelocity ); 
  private:
    enum DrumState{IDLE,WAIT_FOR_MAX,SEND_NOTE_ON,BLOCK,SEND_NOTE_OFF};
    DrumState State;
    int AnalogInputNumber;
    int MidiAddress;
    int ADCToVelocity(int ADCRead);
    int AnalogRead;
    int HighestRead;
    unsigned long HitStartTime;
    unsigned long TimeSinceHit;      
};

#endif
