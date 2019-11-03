#ifndef __PROCESSBLOCK_HPP__
#define __PROCESSBLOCK_HPP__

#include <stdint.h>
#include "SamplingTypes.hpp"
#include "BlockState.hpp"
#include "ProcessBlockFunctions_FIRFilters.hpp"

typedef void (* ProcessBlockFunctionPointer)(BlockState *, sample_t *, sample_t *, uint32_t);

class ProcessBlock{
public:
  virtual void process(sample_t * samplesToProcess) = 0;
  virtual ~ProcessBlock() {};
};

class RealProcessBlock : public ProcessBlock{

  ProcessBlockFunctionPointer processFunc;
  sample_t * inputBuffer;
  sample_t * outputBuffer;
  uint32_t num_samples;

  BlockState * blockState;

public:
  RealProcessBlock(ProcessBlockFunctionPointer func, uint32_t size){
    processFunc = func;
    num_samples = size;
    inputBuffer = new sample_t[size];
    outputBuffer = new sample_t[size];

    blockState = new BlockState();

    //terrible, check function
    if(func == ProcessBlockFunctions_FIRLowPass){
      ProcessBlockFunctions_FIRLowPass_CalculateCoefficients(2000);
    }
  }

  ~RealProcessBlock(){;}

  sample_t * getOutputBuffer(void){
    return outputBuffer;
  }

  void setParam(BlockParamIdentifier_t id, int value){
    blockState -> setParam(id, value);
  }

  void process(sample_t * samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }

    processFunc(blockState, inputBuffer, outputBuffer, num_samples);
  }
};

#endif
