#ifndef DRUM_H
#define DRUM_H

#include "Arduino.h"

#define SNARE_ADDRESS 26
#define HIHAT_ADDRESS 42
#define NOTE_ON (0x90)
#define NOTE_OFF (0x80)
#define NOTE_BUFFER_TIME 5 //in miliseconds
#define NOTE_LENGTH 65 // in miliseconds
#define THRESHOLD 20

class Drum{
	
  public:   
    Drum(int AnalogInputNumber=A4, int MidiAddress=SNARE_ADDRESS);
    void CheckHits(void);
    void SendMidi(int Command, int NoteAddress, int NoteVelocity);
    
   // private:
    int AnalogInputNumber;
    int MidiAddress;
    int ADCToVelocity(int ADCRead);
    int AnalogRead;
    int NextAnalogRead;
    int Velocity;
    bool ActiveHit;
    unsigned long HitStartTime;
    unsigned long TimeSinceHit;      
};

#endif
