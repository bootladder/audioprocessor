#ifndef __DELAYBUFFER_HPP__
#define __DELAYBUFFER_HPP__

#include "SamplingTypes.h"

class DelayBuffer{
  sample_t * delayBuffer;
  int index; //points to where the newest sample is
  int size ;
public:
  DelayBuffer(int size_param){
    delayBuffer = new sample_t[size_param];
    size = size_param;
    index = 0;
  }

  void insertSample(const sample_t sample){
    index++;
    if(index >= size){
      index = 0;
    }

    delayBuffer[index] = sample;
  }

  sample_t getDelayedSample(const int delay){

    int indexToReturn;
    if(delay <= index){
      indexToReturn = (index - delay);
      return delayBuffer[indexToReturn];
    }

    else{
      indexToReturn = (size) - (delay - index);
      return delayBuffer[indexToReturn];
    }

  }
};


#endif
