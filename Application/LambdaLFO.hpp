#ifndef __LAMBDA_LFO_HPP__
#define __LAMBDA_LFO_HPP__

#include <functional>
#include "MIDIReceiver.hpp"
#include "SamplingTypes.h"

class LambdaLFO : public MIDIReceiver{

  const char * name;
  std::function<void(int)> lambda;

  sample_t lfoFreqHz;
  int timerTickPeriodMs;
  int midpoint;
  int amplitude;

  int currentLFOValue;
  int ticks;

public:
  LambdaLFO(const char * name): name(name)
  {
    midpoint = 0;
    amplitude = 0;
  }

  void setLFOFrequencyHz(float freq){ lfoFreqHz = freq; }
  void setTickPeriodMillis(int ms ){ timerTickPeriodMs = ms; }
  void setAmplitude(int a){ amplitude = a; }
  void setMidPoint(int m){ midpoint = m; }
  int getCurrentLFOValue(){ return currentLFOValue; }

  void setLambda(std::function<void(int)> l){
      lambda = l;
  }

  void tickCallback(){
      ticks++;
      int T = (int) (1000.0/lfoFreqHz/timerTickPeriodMs);
      if(ticks >= T)
          ticks = 0;
      int sawToothValue = (-1*amplitude) + (amplitude*ticks*2/T);
      currentLFOValue = (abs(sawToothValue) - (amplitude/2)) + midpoint;

      lambda(currentLFOValue);
  }



  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;
    const sample_t MIDI_VALUES_PER_HZ = 10.0;
    lfoFreqHz = (sample_t)value/MIDI_VALUES_PER_HZ;
  }
};

#endif