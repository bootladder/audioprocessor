#include "ProcessBlock.hpp"

class MixerBlock : public ProcessBlock {
public:
  MixerBlock(const char * name, uint32_t size) :
    ProcessBlock(name, size){
    reset();
  }

  //clear output buf
  void reset(void){
    for(uint32_t i=0; i<num_samples; i++){
      outputBuffer[i] = (sample_t) 0;
    }
  }

  void process(sample_t * samplesToProcess)
  {
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }


    //track max
    sample_t max = 0;
    sample_t min = 0;
    
    for(uint32_t i=0; i<num_samples; i++){
      outputBuffer[i] = outputBuffer[i] + inputBuffer[i];
      if(outputBuffer[i] > max)
        max = outputBuffer[i];
      if(outputBuffer[i] < min)
        min = outputBuffer[i];
    }

    min = min*-1;
    sample_t absmax = max > min ? max : min;

    if(absmax > 32700){
      SerialLogger_LogLiteralString(LOGTYPE_EVENT, "MIXER OVERLOAD\n");
      for(uint32_t i=0; i<num_samples; i++){
        outputBuffer[i] = outputBuffer[i] * (32700/absmax);
      }
    }
  }
};

