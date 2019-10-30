#ifndef __AUDIOPROCESSOR_HPP__
#define __AUDIOPROCESSOR_HPP__

#include "ProcessBlock.hpp"

class AudioProcessor{
  ProcessBlock * block1;
  ProcessBlock * block2;
public:
  AudioProcessor(){
    block1 = 0;
    block2 = 0;
  };
  int16_t * ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples);
  void AddBlockInSeries(ProcessBlock * block);

};

#endif
