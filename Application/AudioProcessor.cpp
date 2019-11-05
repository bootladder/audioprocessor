extern "C" {
#include "AudioProcessor.h"
}

#include "SamplingTypes.hpp"
#include "BSP_Audio_Buffer_Interface.h"
#include "ProcessBlock.hpp"
#include "ProcessBlockFunctions.hpp"
#include "ProcessBlockFunctions_FIRFilters.hpp"
#include "FIRBlock.hpp"

class AudioProcessor{
public:
  void init(void);
  sample_t * process(sample_t * sampleBuf);
};

AudioProcessor audioProcessor;

// This is called by the FreeRTOS Audio Task, every time the DMA receive buf fills up.
// Takes int16_t samples, processes, returns int16_t samples.
// The processor takes sample_t's, currently defined to be floats

// The AudioProcessor MUST be initialized!!
// Make a call to AudioProcessor_Init() !!!
extern "C"
int16_t *
AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  static sample_t inputSampleBuffer[MY_PROCESSING_BUFFER_SIZE_SAMPLES];  //max size
  static int16_t outputInt16Buffer[MY_PROCESSING_BUFFER_SIZE_SAMPLES];

  for(uint32_t i=0; i<num_samples; i++){
    inputSampleBuffer[i] = (sample_t)sampleBuf[i];
  }

  sample_t * out = audioProcessor.process(inputSampleBuffer);

  for(uint32_t i=0; i<num_samples; i++){
    outputInt16Buffer[i] = (int16_t)out[i];
  }

  return outputInt16Buffer;
}

extern "C" void AudioProcessor_Init(void)
{
  audioProcessor.init();
}



static FIRBlock firBlock1(MY_PROCESSING_BUFFER_SIZE_SAMPLES, 200);
static FIRBlock firBlock2(MY_PROCESSING_BUFFER_SIZE_SAMPLES, 800);

static GainBlock gainBlock1(MY_PROCESSING_BUFFER_SIZE_SAMPLES);
static GainBlock gainBlock2(MY_PROCESSING_BUFFER_SIZE_SAMPLES);

static ClippingDistortionBlock clippingBlock1(MY_PROCESSING_BUFFER_SIZE_SAMPLES);

void AudioProcessor::init(void)
{
}

sample_t * AudioProcessor::process(sample_t * sampleBuf)
{
  sample_t * out;

  gainBlock1.setParam(PARAM_0, 4);  //don't set the param every time
  gainBlock1.process(sampleBuf);
  out = gainBlock1.getOutputBuffer();

  clippingBlock1.process(out);
  out = clippingBlock1.getOutputBuffer();

  //  gainBlock2.setParam(PARAM_0, 2);  //don't set the param every time
  //  gainBlock2.process(out);
  //  out = gainBlock2.getOutputBuffer();

  firBlock1.process(out);
  out = firBlock1.getOutputBuffer();

  firBlock2.process(out);
  out = firBlock2.getOutputBuffer();

  return out;
}
