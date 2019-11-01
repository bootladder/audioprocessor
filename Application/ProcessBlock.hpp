#ifndef __PROCESSBLOCK_HPP__
#define __PROCESSBLOCK_HPP__

#include <stdint.h>
#include "BlockState.hpp"

typedef void (* ProcessBlockFunctionPointer)(BlockState *, int16_t *, int16_t *, uint32_t);

class ProcessBlock{

  ProcessBlockFunctionPointer processFunc;
  int16_t * inputBuffer;
  int16_t * outputBuffer;
  uint32_t num_samples;

  BlockState * blockState;

public:
  ProcessBlock(ProcessBlockFunctionPointer func, uint32_t size){
    processFunc = func;
    num_samples = size;
    inputBuffer = new int16_t[size];
    outputBuffer = new int16_t[size];

    blockState = new BlockState();
  }

  int16_t * getOutputBuffer(void){
    return outputBuffer;
  }

  void setParam(BlockParamIdentifier_t id, int value){
    blockState -> setParam(id, value);
  }

  void process(int16_t * samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }

    processFunc(blockState, inputBuffer, outputBuffer, num_samples);
  }
};

#endif
