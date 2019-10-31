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
    nodes[i].addEdge(&nodes[i+1], EDGE_PASSTHROUGH);
  }

  int16_t * out = nodes[0].applySubgraphToSampleBuffer(testBuf, num_samples);

  for(uint32_t i=0; i<num_samples; i++){
    ASSERT_EQ(out[i], (int16_t)NUM_LOTSOFBLOCKS);
  }
}

TEST_F(GraphNodeTest, applySubGraphToSampleBuffer_SplitJoin2Paths)
{
  ProcessBlock block0(DummyProcessFunctions_noop);
  GraphNode node0(&block0);

  ProcessBlock block1A(DummyProcessFunctions_add_1);
  GraphNode node1A(&block1A);

  ProcessBlock block1B(DummyProcessFunctions_add_2);
  GraphNode node1B(&block1B);

  node0.addEdge(&node1A, EDGE_PASSTHROUGH);
  node0.addEdge(&node1B, EDGE_PASSTHROUGH);

  ProcessBlock block2(DummyProcessFunctions_noop);
  GraphNode node2(&block2);

  node1A.addEdge(&node2, EDGE_PASSTHROUGH);
  node1B.addEdge(&node2, EDGE_JOIN_TERMINATOR);


  int16_t * out = node0.applySubgraphToSampleBuffer(testBuf, num_samples);

  for(uint32_t i=0; i<num_samples; i++){
    ASSERT_EQ(3, out[i]);
  }
}
