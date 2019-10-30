#include <iostream>
#include "gtest/gtest.h"
using namespace std;

extern "C"{
  #include "AudioProcessor.h"
}
#include "AudioProcessor.hpp"

#include "ProcessBlock.hpp"
#include "DummyProcessFunctions.hpp"

#define NUM_SAMPLES 256
static uint32_t num_samples = NUM_SAMPLES;
static int16_t testBuf[NUM_SAMPLES];

class AudioProcessorTest: public ::testing::Test {
protected:
  AudioProcessorTest() {
  }
};


TEST_F(AudioProcessorTest, ProcessSampleBuffer_TwoFiltersInSeries_Applies)
{
  AudioProcessor audioProcessor;
  ProcessBlock block1(DummyProcessFunctions_half_volume);
  ProcessBlock block2(DummyProcessFunctions_half_volume);
  audioProcessor.AddBlockInSeries(&block1);
  audioProcessor.AddBlockInSeries(&block2);

  for(uint32_t i=0; i<num_samples; i++){
    testBuf[i] = i*40;
  }

  int16_t * out = audioProcessor.ProcessSampleBuffer(testBuf, num_samples);

  for(uint32_t i=0; i<num_samples; i++){
    ASSERT_EQ(out[i], i*10);
  }
}

TEST_F(AudioProcessorTest, ProcessSampleBuffer_LotsOfFiltersInSeries_AppliesAll)
{
  AudioProcessor audioProcessor;
  ProcessBlock blocks[5];
  for(int i=0; i<5; i++){
    blocks[i] = ProcessBlock(DummyProcessFunctions_add_1);
    audioProcessor.AddBlockInSeries(&blocks[i]);
  }

  for(uint32_t i=0; i<num_samples; i++){
    testBuf[i] = 1;
  }

  int16_t * out = audioProcessor.ProcessSampleBuffer(testBuf, num_samples);

  for(uint32_t i=0; i<num_samples; i++){
    ASSERT_EQ(out[i], 6);
  }
}
