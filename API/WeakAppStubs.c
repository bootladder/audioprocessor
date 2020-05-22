//
// Created by steve on 5/21/20.
//
#include <stdint.h>

#define WEAK __attribute__((weak))
WEAK void AudioProcessor_Init(void){

}
WEAK int16_t * AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  return (int16_t *) 0;
}
