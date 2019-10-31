#ifndef __AUDIOPROCESSOR_HPP__
#define __AUDIOPROCESSOR_HPP__

#include "ProcessBlock.hpp"
#define STATIC_NUM_PROCESS_BLOCKS 5
#include "GraphNode.hpp"

class AudioProcessor{
  ProcessBlock * blocks[STATIC_NUM_PROCESS_BLOCKS];
  int numBlocksAssigned = 0;
  GraphNode * graph = 0;
public:
  AudioProcessor(){
    for(int i=0; i<STATIC_NUM_PROCESS_BLOCKS; i++){
      blocks[i] = 0;
    }
  };
  int16_t * ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples);
  void AddBlockInSeries(ProcessBlock * block);
  void SetGraph(GraphNode * graph);

};

#endif
