#ifndef __LAMBDA_LFO_HPP__
#define __LAMBDA_LFO_HPP__

#include <functional>
#include "LFO.hpp"

class LambdaLFO : public LFO{

    std::function<void(int)> lambda;

    int midpoint;
    int amplitude;

public:
  LambdaLFO(const char * name) : LFO(name)
  {
      midpoint = 0;
      amplitude = 0;
  }

    void setAmplitude(int a){
        amplitude = a;
    }

    void setMidPoint(int m){
        midpoint = m;
    }

    int getCurrentLFOValue(){
        return currentLFOValue;
    }

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
};

#endif