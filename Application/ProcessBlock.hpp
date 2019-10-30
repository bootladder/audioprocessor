#ifndef __PROCESS_BLOCK_HPP__
#define __PROCESS_BLOCK_HPP__
#include <stdint.h>

typedef int16_t * (* ProcessFunctionPtr)(int16_t *, uint32_t num_samples);

class ProcessBlock
{
  ProcessFunctionPtr processFunc;

public:
  ProcessBlock(){}
  ProcessBlock(ProcessFunctionPtr f){
    processFunc = f;
  }

  int16_t * ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples);
};


#endif
