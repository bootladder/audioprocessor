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
  return sampleBuf;
}

int16_t * AudioProcessor::ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  static int16_t * out1 = block1->ProcessSampleBuffer(sampleBuf, num_samples);
  static int16_t * out2 = block2->ProcessSampleBuffer(out1, num_samples);
  return out2;
}

void AudioProcessor::AddBlockInSeries(ProcessBlock * block)
{
  if(block1 == 0)
    block1 = block;
  else
    block2 = block;
}
