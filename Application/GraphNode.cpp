#include "GraphNode.hpp"

void GraphNode::addEdge(GraphNode * node, EdgeType_t edgeType)
{
  edges[numEdges].node = node;
  edges[numEdges].edgeType = edgeType;
  numEdges++;
}


void GraphNode::applyGraphToSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  sampleBuffer = sampleBuf;

  int16_t * nodeOutput = block->ProcessSampleBuffer(sampleBuf, num_samples);

  for(int i=0; i<numEdges; i++){

    switch(edges[i].edgeType){
    case EDGE_JOIN_TERMINATOR:  //don't recurse.  Just apply the next block
      edges[i].node->applyBlockToSampleBuffer(nodeOutput, num_samples);
      break;

    case EDGE_PASSTHROUGH:  //recurse
      edges[i].node->applyGraphToSampleBuffer(nodeOutput, num_samples);
      break;
    }
  }
}


void GraphNode::applyBlockToSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  //assumes the block has internal state
  block->ProcessSampleBuffer(sampleBuf, num_samples);
}
