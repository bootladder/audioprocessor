#include "ProcessBlock.hpp"

int16_t * ProcessBlock::ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  processFunc(sampleBuf, num_samples);
  return sampleBuf;
}
