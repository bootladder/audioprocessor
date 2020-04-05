#ifndef __DELAYBLOCK_HPP__
#define __DELAYBLOCK_HPP__

#include "ProcessBlock.hpp"
extern "C"{
#include "SerialLogger.h"
#include "tinyprintf.h"
}

#include "DelayBuffer.hpp"
class DelayBlock : public ProcessBlock {

  int delayNumSamples;
  int delayNumSamples_lastTimeProcessed; //it can change
  int delayMillis; //for printing
  DelayBuffer * delayBuffer;

public:
  DelayBlock(const char * name, uint32_t size) :
    ProcessBlock(name, size){
    delayBuffer = new DelayBuffer(1024*40);//static 20k
    delayNumSamples = 0;
  }

  //for testing
  int getDelayNumSamples(void){return delayNumSamples;}

  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;
    float delayNumSamples_float = ((float)value/128.0) * 100.0 * (1.0/1000.0) * (48000.0);
    delayNumSamples = (int) delayNumSamples_float;
    delayMillis = (int)(((float)value/128.0) * 100.0);

    static char str[100];
    int size = tfp_snprintf(str,100, "%s, Delay(ms), %d\n", name, delayMillis);
    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
  }


  void process(sample_t * samplesToProcess)
  {
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }

    if(delayNumSamples_lastTimeProcessed == delayNumSamples){
      //add delayed input to input
      //add sample to delayBuffer
      for(uint32_t i=0; i<num_samples; i++){
        //outputBuffer[i] = inputBuffer[i]
        //  + 0.7* delayBuffer->getDelayedSample(delayNumSamples)
        //  //+ 0.5* delayBuffer->getDelayedSample(delayNumSamples*2)
        //  //+ 0.3* delayBuffer->getDelayedSample(delayNumSamples*3)
        //  //+ 0.2* delayBuffer->getDelayedSample(delayNumSamples*4)
        //  //+ 0.1* delayBuffer->getDelayedSample(delayNumSamples*5)
        //  //+ 0.1* delayBuffer->getDelayedSample(delayNumSamples*6)
        //  ;
        outputBuffer[i] = getSampleAtIndex(i, delayNumSamples);
        delayBuffer->insertSample(inputBuffer[i]);
      }

    }
    else{
      for(uint32_t i=0; i<num_samples; i++){
        float slope = ((float)delayNumSamples - (float)delayNumSamples_lastTimeProcessed) / (float)num_samples;
        int interpolatedDelayNumSamples = 
          delayNumSamples_lastTimeProcessed +
            i*(slope);

        outputBuffer[i] = getSampleAtIndex(i, interpolatedDelayNumSamples);

        //outputBuffer[i] = inputBuffer[i]
        //  + 0.7* delayBuffer->getDelayedSample(interpolatedDelayNumSamples)
        //  ;
        delayBuffer->insertSample(inputBuffer[i]);
      }

    }
    
    delayNumSamples_lastTimeProcessed = delayNumSamples;
  }

  sample_t getSampleAtIndex(int i, int delayNumSamples){
        return  inputBuffer[i]
          + 0.7* delayBuffer->getDelayedSample(delayNumSamples)
          + 0.5* delayBuffer->getDelayedSample(delayNumSamples*2)
          + 0.3* delayBuffer->getDelayedSample(delayNumSamples*3)
          + 0.2* delayBuffer->getDelayedSample(delayNumSamples*4)
          + 0.1* delayBuffer->getDelayedSample(delayNumSamples*5)
          + 0.1* delayBuffer->getDelayedSample(delayNumSamples*6);
  }
};

#endif
