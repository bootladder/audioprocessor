#include <iostream>
#include "gtest/gtest.h"

#include "ProcessBlock.hpp"
#include "ProcessBlockFunctions.hpp"

#define NUM_SAMPLES 1024
static int16_t testBuf[NUM_SAMPLES];

static void init_testBuf_with_staircase(void){
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    testBuf[i] = i;
  }
}

TEST(ProcessBlock, identity)
{
  ProcessBlock block(ProcessBlockFunctions_Identity, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.process(testBuf);

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((int16_t)i, testBuf[i]);
  }
}

TEST(ProcessBlock, gain2x)
{
  ProcessBlock block(ProcessBlockFunctions_Gain2X, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.process(testBuf);
  int16_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((int16_t)i*2, out[i]);
  }
}

TEST(ProcessBlock, gainParameterized)
{
  ProcessBlock block(ProcessBlockFunctions_GainParameterized, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.setParam(PARAM_0, 3);
  block.process(testBuf);
  int16_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((int16_t)i*3, out[i]);
  }
}
