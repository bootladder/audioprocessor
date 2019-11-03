extern "C" {
#include "AudioProcessor.h"
}

#include "SamplingTypes.hpp"
#include "BSP_Audio_Buffer_Interface.h"
#include "ProcessBlock.hpp"
#include "ProcessBlockFunctions.hpp"
#include "ProcessBlockFunctions_FIRFilters.hpp"

static sample_t inputSampleBuffer[MY_PROCESSING_BUFFER_SIZE_SAMPLES];
static int16_t outputInt16Buffer[MY_PROCESSING_BUFFER_SIZE_SAMPLES];

static sample_t * __testing__process_sample_buffer(sample_t * sampleBuf);

extern "C"
int16_t *
AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  (void)num_samples;

  for(uint32_t i=0; i<MY_PROCESSING_BUFFER_SIZE_SAMPLES; i++){
    inputSampleBuffer[i] = (sample_t)sampleBuf[i];
  }

  sample_t * out = __testing__process_sample_buffer(inputSampleBuffer);


  for(uint32_t i=0; i<MY_PROCESSING_BUFFER_SIZE_SAMPLES; i++){
    outputInt16Buffer[i] = (int16_t)out[i];
  }
  return outputInt16Buffer;
}



static RealProcessBlock block1(ProcessBlockFunctions_GainParameterized, MY_PROCESSING_BUFFER_SIZE_SAMPLES);
static RealProcessBlock block4(ProcessBlockFunctions_ClippingDistortion, MY_PROCESSING_BUFFER_SIZE_SAMPLES);
static RealProcessBlock block5(ProcessBlockFunctions_FIRLowPass, MY_PROCESSING_BUFFER_SIZE_SAMPLES);

static sample_t * __testing__process_sample_buffer(sample_t * sampleBuf)
{
  sample_t * out;
  block1.process(sampleBuf);
  out = block1.getOutputBuffer();
  block1.setParam(PARAM_0, 2);

  block4.process(out);
  out = block4.getOutputBuffer();

  block5.process(out);
  out = block5.getOutputBuffer();

  return out;
}
