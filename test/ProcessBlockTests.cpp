#include <iostream>
#include "gtest/gtest.h"

#include "ProcessBlock.hpp"
#include "DummyProcessFunctions.hpp"


TEST(ProcessBlock, half_volume)
{
  int testBufferSize = 256;
  int16_t testBuf[testBufferSize];

  for(int i=0; i<testBufferSize; i++){
    testBuf[i] = i*10;
  }

  ProcessBlock block(DummyProcessFunctions_half_volume);
  static int16_t * out = block.ProcessSampleBuffer(testBuf, testBufferSize);

  for(int i=0; i<testBufferSize; i++){
    ASSERT_EQ(out[i], i*5);
  }
}

TEST(ProcessBlock, apply_2_filters_half_volume_results_in_quarter_volume)
{
  ProcessBlock block1(DummyProcessFunctions_half_volume);
  ProcessBlock block2(DummyProcessFunctions_half_volume);

  int testBufferSize = 256;
  int16_t testBuf[testBufferSize];

  //i*40
  for(int i=0; i<testBufferSize; i++){
    testBuf[i] = i*40;
  }

  static int16_t * out1 = block1.ProcessSampleBuffer(testBuf, testBufferSize);
  static int16_t * out2 = block2.ProcessSampleBuffer(out1, testBufferSize);

  for(int i=0; i<testBufferSize; i++){
    ASSERT_EQ(out2[i], i*10);
  }
}
