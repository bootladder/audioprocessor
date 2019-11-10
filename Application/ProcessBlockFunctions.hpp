#ifndef __PROCESSBLOCKFUNCTIONS_HPP__
#define __PROCESSBLOCKFUNCTIONS_HPP__
#include <stdint.h>

#include "SamplingTypes.hpp"
#include "BlockState.hpp"

void ProcessBlockFunctions_Identity(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_Gain2X(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_GainParameterized(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_Attenuation(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_ClippingDistortion(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_Mixer(BlockState * state, sample_t * in, sample_t * out, uint32_t size);


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

  void insertSample(sample_t sample){
    index++;
    if(index >= size){
      index = 0;
    }

    delayBuffer[index] = sample;
  }

  sample_t getDelayedSample(int delay){

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
