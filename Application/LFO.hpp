#ifndef __LFO_HPP__
#define __LFO_HPP__

#include "MIDI_Message.hpp"
#include "MIDIReceiver.hpp"


extern "C"{
#include "MIDIMessageHandler.h"
}

class LFO;

typedef void (*MIDIMessageHandlerFunc_t)(MIDI_Message_t);
typedef void (*StartTimerFunc)(LFO & lfo, int);

class LFO : public MIDIReceiver{

protected:
    const char * name;
    float lfoFreqHz;
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
    lfoFreqHz = 1;
  }

  void setLFOFrequencyHz(float freq){ lfoFreqHz = freq; }

  void setMIDIMessage(MIDI_Message_t msg){ midiMessage = msg; }

  void setMIDIMessageHandlerFunc(MIDIMessageHandlerFunc_t func){
    midiMessageHandlerFunc = func;
  }

  void setStartTimerMsFunc(StartTimerFunc func){
      startTimerFunc = func;
  }

  virtual void tickCallback(void){
      ticks++;
      int T = (int) ( 1000.0 / lfoFreqHz/ (float)timerTickPeriodMs);
      if(ticks >= T)
        ticks = 0;
      int sawToothValue = (-127) + (127*ticks*2/T);
      currentLFOValue = abs(sawToothValue);
      midiMessage.value = currentLFOValue;
      midiMessageHandlerFunc(midiMessage);
  }

  void startTimerMs(int ms){
      timerTickPeriodMs = ms;
      startTimerFunc(*this, ms); 
  }

    float getLFOFrequency() {
        return lfoFreqHz;
    }


    virtual void setMIDIParameter(BlockParamIdentifier_t id, int value){
        (void)id;
        lfoFreqHz = value/10.0;
    }


};

#endif