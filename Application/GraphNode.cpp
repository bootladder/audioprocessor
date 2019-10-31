#include "GraphNode.hpp"

void GraphNode::addChild(GraphNode * node)
{
  childs[numChilds] = node;
  numChilds++;
}


int16_t * GraphNode::applySubgraphToSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  int16_t * out = block->ProcessSampleBuffer(sampleBuf, num_samples);

  if(numChilds == 0)
    return out;
  else
    return childs[0]->applySubgraphToSampleBuffer(out,num_samples);
}

/////////////////////////////////////////////////////////
// process function modifies buffer in place
// need to give a copy of the output to each child
//int16_t bufferForNode1A[num_samples];
//for(uint32_t i=0; i<num_samples; i++){
//  bufferForNode1A[i] = out[i];
// }
//
//int16_t * out1A = childs[0]->block->ProcessSampleBuffer(bufferForNode1A, num_samples);
//
//int16_t bufferForNode1B[num_samples];
//for(uint32_t i=0; i<num_samples; i++){
//  bufferForNode1B[i] = out[i];
// }
//
//int16_t * out1B = childs[1]->block->ProcessSampleBuffer(bufferForNode1B, num_samples);
//
//
////add childs
//for(uint32_t i=0; i<num_samples; i++){
//  out[i] = out1A[i] + out1B[i];
// }
