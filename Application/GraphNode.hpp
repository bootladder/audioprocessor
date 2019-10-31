#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__
#include "ProcessBlock.hpp"

#define STATIC_NUM_GRAPHNODE_CHILDS 10

class GraphNode
{
  ProcessBlock * block;

  GraphNode * childs[STATIC_NUM_GRAPHNODE_CHILDS];
  int numChilds = 0;

public:
  GraphNode(){}
  GraphNode(ProcessBlock * blockArg){
    block = blockArg;
  }

  void addChild(GraphNode * node);

  int16_t * applySubgraphToSampleBuffer(int16_t * sampleBuf, uint32_t num_samples);
};

#endif
