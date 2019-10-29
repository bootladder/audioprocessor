extern "C" {
#include "AudioProcessor.h"
}

static int16_t dummyBuf[1024];
static int16_t add_something(uint32_t i);


extern "C"
int16_t *
AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  for(uint32_t i=0; i < num_samples; i++){
    dummyBuf[i] = sampleBuf[i] + add_something(num_samples);
  }
  return dummyBuf;
}

static int16_t add_something(uint32_t i)
{
  return 0;
}
