#include <iostream>
#include "gtest/gtest.h"
using namespace std;

extern "C"{
  #include "AudioProcessor.h"
}
#include "AudioProcessor.hpp"

#include "ProcessBlock.hpp"
#include "DummyProcessFunctions.hpp"
#include "GraphNode.hpp"

#define NUM_SAMPLES 256
static uint32_t num_samples = NUM_SAMPLES;
static int16_t testBuf[NUM_SAMPLES];

class AudioProcessorTest: public ::testing::Test {
protected:
  AudioProcessorTest() {
    for(uint32_t i=0; i<num_samples; i++){
      testBuf[i] = 0;
    }
  }
};


TEST_F(AudioProcessorTest, ProcessSampleBuffer_LotsOfFiltersInSeries_AppliesAll)
{
  GTEST_SKIP();



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

//TEST_F(AudioProcessorTest, ProcessSampleBuffer_SplitJoin2Paths)
//{
//  GTEST_SKIP();
//
//
//  AudioProcessor audioProcessor;
//  ProcessBlock block0(DummyProcessFunctions_noop);
//  GraphNode node0(&block0);
//
//  ProcessBlock block1A(DummyProcessFunctions_add_1);
//  GraphNode node1A(&block1A);
//
//  ProcessBlock block1B(DummyProcessFunctions_add_2);
//  GraphNode node1B(&block1B);
//
//  node0.addChild(&node1A);
//  node0.addChild(&node1B);
//
//  ProcessBlock block2(DummyProcessFunctions_noop);
//  GraphNode node2(&block2);
//
//  node1A.addChild(&node2);
//  node1B.addChild(&node2);
//
//
//  audioProcessor.SetGraph(&node0);
//
//  for(uint32_t i=0; i<num_samples; i++){
//    testBuf[i] = 1;
//  }
//
//  int16_t * out = audioProcessor.ProcessSampleBuffer(testBuf, num_samples);
//
//  for(uint32_t i=0; i<num_samples; i++){
//    ASSERT_EQ(out[i], (int16_t)i*10);
//  }
//}
