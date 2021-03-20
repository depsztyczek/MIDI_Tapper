#ifndef DRUM_H
#define DRUM_H

#include "Arduino.h"

//MIDI addresses
#define SNARE_ADDRESS 26
#define HIHAT_ADDRESS 42
#define KICK_ADDRESS 24
#define FLOOR_ADDRESS 35
#define CRASH_ADDRESS 52

//MIDI commands
#define NOTE_ON (0x90)
#define NOTE_OFF (0x80)

//Control
#define NOTE_BUFFER_TIME 1 //in miliseconds
#define NOTE_LENGTH 65 // in miliseconds
#define THRESHOLD 20   // in ADC value - 0-1023

class Drum{
	
  public:   
    Drum(int AnalogInputNumber=A4, int MidiAddress=SNARE_ADDRESS);
    void CheckHits(void);
    void SendMidi(int Command, int NoteAddress, int NoteVelocity);
    
    private:
    enum DrumState{IDLE,WAIT_FOR_MAX,SEND_NOTE_ON,BLOCK,SEND_NOTE_OFF};
    DrumState NextState;
    int AnalogInputNumber;
    int MidiAddress;
    int ADCToVelocity(int ADCRead);
    int AnalogRead;
    unsigned long HitStartTime;
    unsigned long TimeSinceHit;      
};

#endif
