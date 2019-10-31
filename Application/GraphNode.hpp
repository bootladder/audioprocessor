#ifndef __GRAPH_HPP__
#define __GRAPH_HPP__
#include "ProcessBlock.hpp"

#define STATIC_NUM_GRAPHNODE_EDGES 10

typedef enum {
  EDGE_OUTPUT_TERMINATOR,
  EDGE_JOIN_TERMINATOR,
  EDGE_PASSTHROUGH,
} EdgeType_t;

class GraphNode;

struct Edge
{
  GraphNode * node;
  EdgeType_t edgeType;
};

class GraphNode
{
  ProcessBlock * block;
  int16_t * sampleBuffer;

  Edge edges[STATIC_NUM_GRAPHNODE_EDGES];
  int numEdges = 0;

public:
  GraphNode(){
    edges[0].edgeType = EDGE_OUTPUT_TERMINATOR;
  }
  GraphNode(ProcessBlock * blockArg){
    edges[0].edgeType = EDGE_OUTPUT_TERMINATOR;
    block = blockArg;
  }

  int16_t * getSampleBuffer(void){
    return sampleBuffer;
  }

  void addEdge(GraphNode * node, EdgeType_t edgeType);

  void applyGraphToSampleBuffer(int16_t * sampleBuf, uint32_t num_samples);
  void applyBlockToSampleBuffer(int16_t * sampleBuf, uint32_t num_samples);
};

#endif
