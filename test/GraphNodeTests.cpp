#include <iostream>
#include "gtest/gtest.h"
using namespace std;

#include "GraphNode.hpp"

#include "ProcessBlock.hpp"
#include "DummyProcessFunctions.hpp"

#define NUM_SAMPLES 256
static uint32_t num_samples = NUM_SAMPLES;
static int16_t testBuf[NUM_SAMPLES];

class GraphNodeTest: public ::testing::Test {
protected:
  GraphNodeTest() {
    for(uint32_t i=0; i<num_samples; i++){
      testBuf[i] = 0;
    }
  }
};


TEST_F(GraphNodeTest, applySubgraphToSampleBuffer_OneBlock_AppliesBlock)
{
  ProcessBlock block(DummyProcessFunctions_add_1);
  GraphNode node(&block);

  int16_t * out = node.applySubgraphToSampleBuffer(testBuf, num_samples);

  for(uint32_t i=0; i<num_samples; i++){
    ASSERT_EQ(out[i], (int16_t)1);
  }
}


TEST_F(GraphNodeTest, applySubgraphToSampleBuffer_LotsOfBlocksInSeries)
{
  #define NUM_LOTSOFBLOCKS 10
  ProcessBlock blocks[NUM_LOTSOFBLOCKS];
  GraphNode nodes[NUM_LOTSOFBLOCKS];
  for(uint32_t i=0; i<NUM_LOTSOFBLOCKS; i++){
    blocks[i] = ProcessBlock(DummyProcessFunctions_add_1);
    nodes[i] = GraphNode(&blocks[i]);
  }

  for(uint32_t i=0; i<NUM_LOTSOFBLOCKS - 1; i++){
    nodes[i].addChild(&nodes[i+1]);
  }

  int16_t * out = nodes[0].applySubgraphToSampleBuffer(testBuf, num_samples);

  for(uint32_t i=0; i<num_samples; i++){
    ASSERT_EQ(out[i], (int16_t)NUM_LOTSOFBLOCKS);
  }
}
