#ifndef __LFO_HPP__
#define __LFO_HPP__

#include "MIDI_Message.hpp"
#include "MIDIReceiver.hpp"
#include "SamplingTypes.h"

extern "C"{
#include "MIDIMessageHandler.h"
}

class LFO;

typedef void (*MIDIMessageHandlerFunc_t)(MIDI_Message_t);
typedef void (*StartTimerFunc)(LFO & lfo, int);

class LFO : public MIDIReceiver{

protected:
    const char * name;
    sample_t lfoFreqHz;
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

  virtual void tickCallback(){
      ticks++;
      int T = (int) ( 1000.0 / lfoFreqHz/ (float)timerTickPeriodMs);
      if(ticks >= T)
        ticks = 0;
      int sawToothValue = (-127) + (127*ticks*2/T);
      currentLFOValue = abs(sawToothValue);
      midiMessage.value = (uint8_t) currentLFOValue;
      midiMessageHandlerFunc(midiMessage);
  }

  void startTimerMs(int ms){
      timerTickPeriodMs = ms;
      startTimerFunc(*this, ms); 
  }

    sample_t getLFOFrequency() {
        return lfoFreqHz;
    }


    virtual void setMIDIParameter(BlockParamIdentifier_t id, int value){
        (void)id;
        const sample_t MIDI_VALUES_PER_HZ = 10.0;
        lfoFreqHz = value/MIDI_VALUES_PER_HZ;
    }


};

#endif