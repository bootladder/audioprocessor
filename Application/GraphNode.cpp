#include "GraphNode.hpp"

void GraphNode::addEdge(GraphNode * node, EdgeType_t edgeType)
{
  edges[numEdges].node = node;
  edges[numEdges].edgeType = edgeType;
  numEdges++;
}


int16_t * GraphNode::applyGraphToSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  int16_t * nodeOutput = block->ProcessSampleBuffer(sampleBuf, num_samples);

  for(int i=0; i<numEdges; i++){
    edges[i].node->applyGraphToSampleBuffer(nodeOutput, num_samples);
  }

  return nodeOutput;
}

