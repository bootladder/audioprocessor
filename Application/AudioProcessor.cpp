extern "C" {
#include "AudioProcessor.h"
}


#include "BSP_Audio_Buffer_Interface.h"
#include "ProcessBlock.hpp"
#include "ProcessBlockFunctions.hpp"
static int16_t * __testing__process_sample_buffer(int16_t * sampleBuf);

extern "C"
int16_t *
AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  (void)num_samples;
  int16_t * out = __testing__process_sample_buffer(sampleBuf);
  return out;
}



static ProcessBlock block1(ProcessBlockFunctions_GainParameterized, MY_PROCESSING_BUFFER_SIZE_SAMPLES);
static ProcessBlock block4(ProcessBlockFunctions_ClippingDistortion, MY_PROCESSING_BUFFER_SIZE_SAMPLES);

static int16_t * __testing__process_sample_buffer(int16_t * sampleBuf)
{
  int16_t * out;
  block1.process(sampleBuf);
  out = block1.getOutputBuffer();
  block1.setParam(PARAM_0, 2);

  block4.process(out);
  out = block4.getOutputBuffer();

  return out;
}
