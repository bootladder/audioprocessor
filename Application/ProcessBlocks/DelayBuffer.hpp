#ifndef __DELAYBUFFER_HPP__
#define __DELAYBUFFER_HPP__

#include "SamplingTypes.h"

#define EXT_RAM_SECTION __attribute__((section(".external_ram")))
static EXT_RAM_SECTION sample_t delayBuffer[480000];  //static allocate //for now

class DelayBuffer{
  //sample_t delayBuffer[48000];  //static allocate //for now
  int index; //points to where the newest sample is
  int size ;


public:
  DelayBuffer(int size_param){
    //size = size_param;
    size = 480000;
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
