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
    #define MAX_DELAY_MILLIS 100.0
    #define MILLIS_TO_SECONDS (1.0/1000.0)
    #define SAMPLES_PER_SECOND 48000.0
    #define MAX_INPUT_VALUE 128.0
    const sample_t VALUE_TO_SAMPLES = (MAX_DELAY_MILLIS * MILLIS_TO_SECONDS * SAMPLES_PER_SECOND / MAX_INPUT_VALUE);

    sample_t delayNumSamples_float = (sample_t)value * VALUE_TO_SAMPLES;
    delayNumSamples =  (int)delayNumSamples_float;

    static char str[100];
    int size = tfp_snprintf(str,100, "%s, Delay(ms), %d\n", name, delayMillis);
    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
  }


  void process(const sample_t *samplesToProcess)
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
        outputBuffer[i] = computeSampleAtIndex(i, delayNumSamples);
        delayBuffer->insertSample(inputBuffer[i]);
      }

    }
    else{
      for(uint32_t i=0; i<num_samples; i++){
        sample_t slope = ((sample_t)delayNumSamples - (sample_t)delayNumSamples_lastTimeProcessed) / (sample_t)num_samples;
        int interpolatedDelayNumSamples = delayNumSamples_lastTimeProcessed + (int)((sample_t)i*slope);

        outputBuffer[i] = computeSampleAtIndex(i, interpolatedDelayNumSamples);
        delayBuffer->insertSample(inputBuffer[i]);
      }

    }
    
    delayNumSamples_lastTimeProcessed = delayNumSamples;
  }

  sample_t computeSampleAtIndex(uint32_t i, int delayNumSamples){
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
