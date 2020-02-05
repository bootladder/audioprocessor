#include "ProcessBlock.hpp"
extern "C"{
#include "MemoryLogger.h"
#include "SerialLogger.h"
#include "tinyprintf.h"
}

class ClippingDistortionBlock : public ProcessBlock{

public:
  enum ClippingType {
                     CLIPPING_TYPE_SOFT,
                     CLIPPING_TYPE_HARD
  };

private:
  float clipping_percent;
  ClippingType clipping_type;

public:


  ClippingDistortionBlock(const char * name, uint32_t size, ClippingType clippingType) :
    ProcessBlock(name, size){
    clipping_percent = 0.5;
    clipping_type = clippingType;
  }

  void process(sample_t * samplesToProcess)
  {
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }

    if(clipping_type == CLIPPING_TYPE_HARD)
      clip_hard();
    else
      clip_soft();
  }

  void clip_hard(void)
  {
    //clip half way, both ends
    sample_t max = (sample_t) ((float)0x8000 * (clipping_percent));
    sample_t min = (sample_t) ((float) -0x8000 * (clipping_percent));

    for(uint32_t i=0; i<num_samples; i++){
      if(inputBuffer[i] > max)
        outputBuffer[i] = max;
      else if(inputBuffer[i] < min)
        outputBuffer[i] = min;
      else
        outputBuffer[i] = inputBuffer[i];
    }
  }

  void clip_soft(void)
  {
    //Normalize and Waveshape
    //16 bit signed samples max out at 32000.
    //Wave shaping formula clips after max input
    // So scale down to.... 0.5?  divide by 64000?

    sample_t factor = 16000.0;
    sample_t limit = 0.66666666;
    for(uint32_t i=0; i<num_samples; i++){


      sample_t normalized = inputBuffer[i]/factor;  //normalize to 1 so the cubic works
      sample_t shaped;

      //clip
      if(normalized > limit){
        shaped = limit;
      }
      else if(normalized < (-1*limit)){
        shaped = -1*limit;
      }
      else{
        //shape
        shaped = normalized - (normalized*normalized *normalized / 3);
      }

      sample_t denormalized = shaped * factor;
      outputBuffer[i] = denormalized;
    }
  }


  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;

    clipping_percent = ((float)value/128.0);
    int clipping_percent_int = (int) (clipping_percent*100.0);
    static char str[100];
    int size = tfp_snprintf(str,100, "%s, Clip(%%), %d\n", name, clipping_percent_int);
    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
  }
};
