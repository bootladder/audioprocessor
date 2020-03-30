#include <iostream>
using namespace std;
#include "gtest/gtest.h"

#include "ProcessBlock.hpp"
#include "SuperSimpleProcessBlocks.hpp"
#include "DelayBlock.hpp"
#include "MixerBlock.hpp"

#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];

static void init_testBuf_with_staircase(void){
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    testBuf[i] = i;
  }
}

TEST(GainBlock, gainParameterized)
{
  GainBlock block("name", NUM_SAMPLES);

  init_testBuf_with_staircase();

  // the gain formula is 8.0 * value / 128.0
  block.setMIDIParameter(PARAM_0, 128);
  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*8, out[i]);
  }
}

TEST(ProcessBlock, attenuation)
{
  GainBlock block("name", NUM_SAMPLES);

  init_testBuf_with_staircase();

  // the attenuation formula is 2.0 * value / 128.0
  block.setGainFactor(2.0);
  block.setMIDIParameter(PARAM_0, 64);
  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*1, out[i]);
  }
}

TEST(ProcessBlock, Mixer_2_Inputs_Mixes)
{
  MixerBlock mixerBlock("name", NUM_SAMPLES);

  sample_t inputBuf1[NUM_SAMPLES];
  sample_t inputBuf2[NUM_SAMPLES];

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    inputBuf1[i] = 2;
    inputBuf2[i] = 3;
  }

  mixerBlock.process(inputBuf1);
  mixerBlock.process(inputBuf2);

  sample_t * out = mixerBlock.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ(out[i], 5);
  }
}

//need to clear the output buffer between subsequent mixes
//for now, add a reset() method
TEST(ProcessBlock, Mixer_CalledTwice_Works)
{
  MixerBlock mixerBlock("name", NUM_SAMPLES);

  sample_t inputBuf1[NUM_SAMPLES];
  sample_t inputBuf2[NUM_SAMPLES];
  sample_t * out;

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    inputBuf1[i] = 2;
    inputBuf2[i] = 3;
  }

  mixerBlock.process(inputBuf1);
  mixerBlock.process(inputBuf2);

  out = mixerBlock.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ(out[i], 5);
  }

  //reset the mixer block
  mixerBlock.reset();
  // do the same thing again

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    inputBuf1[i] = 2;
    inputBuf2[i] = 3;
  }

  mixerBlock.process(inputBuf1);
  mixerBlock.process(inputBuf2);

  out = mixerBlock.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ(out[i], 5);
  }
}
