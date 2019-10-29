extern "C" {

#include "AudioProcessor.h"

static int16_t dummyBuf[1024];

int16_t * AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  for(uint32_t i=0; i < num_samples; i++){
    dummyBuf[i] = sampleBuf[i];
  }
  return dummyBuf;
}

}
