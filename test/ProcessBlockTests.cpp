#include <iostream>
using namespace std;
#include "gtest/gtest.h"

#include "ProcessBlock.hpp"
#include "ProcessBlockFunctions.hpp"

#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];

static void init_testBuf_with_staircase(void){
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    testBuf[i] = i;
  }
}

TEST(ProcessBlock, identity)
{
  RealProcessBlock block(ProcessBlockFunctions_Identity, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.process(testBuf);

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i, testBuf[i]);
  }
}

TEST(ProcessBlock, gain2x)
{
  RealProcessBlock block(ProcessBlockFunctions_Gain2X, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*2, out[i]);
  }
}

TEST(ProcessBlock, gainParameterized)
{
  RealProcessBlock block(ProcessBlockFunctions_GainParameterized, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.setParam(PARAM_0, 3);
  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*3, out[i]);
  }
}

TEST(ProcessBlock, clippingdistortion_DefaultSettings_ClipsAtHalfAmplitudePositiveAndNegative)
{
  RealProcessBlock block(ProcessBlockFunctions_ClippingDistortion, NUM_SAMPLES);

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    testBuf[i] = -0x8000 + (i*0x10000)/NUM_SAMPLES;
  }

  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_LE(out[i], 0x8000/2);
    ASSERT_GE(out[i], -(0x8000/2));
  }
}
