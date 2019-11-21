#include "ProcessBlock.hpp"

class DelayBuffer{
  sample_t * delayBuffer;
  int index; //points to where the newest sample is
  int size ;
public:
  DelayBuffer(int size_param){
    delayBuffer = new sample_t[size_param];
    size = size_param;
    index = 0;
  }

  void insertSample(sample_t sample){
    index++;
    if(index >= size){
      index = 0;
    }

    delayBuffer[index] = sample;
  }

  sample_t getDelayedSample(int delay){

    int indexToReturn;
    if(delay <= index){
      indexToReturn = (index - delay);
      return delayBuffer[indexToReturn];
    }

    else{
      indexToReturn = (size) - (delay - index);
      return delayBuffer[indexToReturn];
    }

  }
};

class DelayBlock : public ProcessBlock {

  int delayNumSamples;
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

    //add delayed input to input
    //add sample to delayBuffer
    for(uint32_t i=0; i<num_samples; i++){
      outputBuffer[i] = inputBuffer[i]
        + 0.7* delayBuffer->getDelayedSample(delayNumSamples)
        + 0.5* delayBuffer->getDelayedSample(delayNumSamples*2)
        + 0.3* delayBuffer->getDelayedSample(delayNumSamples*3)
        + 0.2* delayBuffer->getDelayedSample(delayNumSamples*4)
        + 0.1* delayBuffer->getDelayedSample(delayNumSamples*5)
        + 0.1* delayBuffer->getDelayedSample(delayNumSamples*6)
        ;
      delayBuffer->insertSample(inputBuffer[i]);
    }
  }
};
