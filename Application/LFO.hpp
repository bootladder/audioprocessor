#ifndef __LFO_HPP__
#define __LFO_HPP__

#include "MIDI_Message.hpp"


extern "C"{
#include "MIDIMessageHandler.h"
}

class LFO;

typedef void (*MIDIMessageHandlerFunc_t)(MIDI_Message_t);
//typedef void (LFO::*LFOMethod_t) (void);
typedef void (*StartTimerFunc)(LFO * lfo, int);

class LFO {

    const char * name;
    int lfoFreqHz;
    int midiMessageFreqHz;
    int timerTickPeriodMs;
    int currentLFOValue;
    int ticks;
    MIDI_Message_t midiMessage;
    MIDIMessageHandlerFunc_t midiMessageHandlerFunc;
    StartTimerFunc startTimerFunc;

public:
  LFO(const char * name)
  {
    midiMessageHandlerFunc = MIDIMessageHandler_Handle;
    //startTimerFunc = 
    name = name;
    currentLFOValue = 0;
    ticks = 0;
  }

  void setLFOFrequencyHz(int freq){ lfoFreqHz = freq; }

  void setMIDIMessageFrequencyHz(int freq){ midiMessageFreqHz = freq; }

  void setMIDIMessage(MIDI_Message_t msg){ midiMessage = msg; }

  void setMIDIMessageHandlerFunc(MIDIMessageHandlerFunc_t func){
    midiMessageHandlerFunc = func;
  }

  void setStartTimerMsFunc(StartTimerFunc func){
      startTimerFunc = func;
  }

  void tickCallback(void){
      ticks++;
      int sawToothValue = (-127/2) + (127*ticks*timerTickPeriodMs/(1000/lfoFreqHz));
      currentLFOValue = abs(sawToothValue)*2;
      midiMessage.value = currentLFOValue;
      midiMessageHandlerFunc(midiMessage);
  }

  void startTimerMs(int ms){
      timerTickPeriodMs = ms;
      startTimerFunc(this, ms); 
  }
};

#endif