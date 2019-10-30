extern "C" {
#include "AudioProcessor.h"
}
#include "AudioProcessor.hpp"

#include "ProcessBlock.hpp"
#include "DummyProcessFunctions.hpp"

ProcessBlock block(DummyProcessFunctions_half_volume);


extern "C"
int16_t *
AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  (void)num_samples;
  return sampleBuf;
}

int16_t * AudioProcessor::ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  int16_t * in = sampleBuf;
  for(int i=0; i<numBlocksAssigned; i++){
    in = blocks[i]->ProcessSampleBuffer(in, num_samples);
  }
  return in;
}

void AudioProcessor::AddBlockInSeries(ProcessBlock * block)
{
  blocks[numBlocksAssigned] = block;
  numBlocksAssigned++;
}