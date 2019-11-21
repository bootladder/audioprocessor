#ifndef __PROCESSBLOCK_HPP__
#define __PROCESSBLOCK_HPP__

#include <stdint.h>
#include "SamplingTypes.h"
#include "MIDI_Message.h"

extern "C"{
#include "SerialLogger.h"
#include "tinyprintf.h"
}

enum BlockParamIdentifier_t{
  PARAM_0 = 0,
  PARAM_1 = 1,
};

class MIDIAssignment{
public:
  MIDI_Message_t msg;
  BlockParamIdentifier_t paramId;
};

class ProcessBlock{

protected:
  const char * name;
  MIDIAssignment midiAssignments[16];  //hard coded
  int midiAssignmentIndex = 0;
  sample_t * inputBuffer;
  sample_t * outputBuffer;
  uint32_t num_samples;

public:

  ProcessBlock(const char * name_param, uint32_t size){
    name = name_param;
    inputBuffer = new sample_t[size];
    outputBuffer = new sample_t[size];
    num_samples = size;
  }
  virtual ~ProcessBlock() {
    delete []inputBuffer;
    delete []outputBuffer;
  };

  const char * getName(void) {return name;}
  sample_t * getOutputBuffer(void){return outputBuffer;}


  //default process function is identity (pass thru)
  virtual void process(sample_t * samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
      outputBuffer[i] = inputBuffer[i];
    }
  }


  void assignMIDIMessageToParameter(MIDI_Message_t & msg, BlockParamIdentifier_t id){
    midiAssignments[midiAssignmentIndex].msg = msg;
    midiAssignments[midiAssignmentIndex].paramId = id;
    midiAssignmentIndex++;
  }

  virtual void MIDIMessageReceived(MIDI_Message_t & msg){
    for(int i=0; i<midiAssignmentIndex; i++){
      if(midiAssignments[i].msg.type != msg.type)
        continue;
      if(midiAssignments[i].msg.id != msg.id)
        continue;

      setMIDIParameter(midiAssignments[i].paramId, msg.value);
    }
  }

  virtual void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;(void)value;
  }

};


class GainBlock : public ProcessBlock{
  float gain;
  float gainFactor;
public:
  GainBlock(const char * name, uint32_t size) :
    ProcessBlock(name, size){
    gain = 1.0;
    gainFactor = 8.0;
  }

  void setGainFactor(float factor){
    gainFactor = factor;
  }
  char * paramToString(BlockParamIdentifier_t id){
    (void)id;
    static char str[10];
    int gainFirstDigit = (int)gain;
    tfp_snprintf(str, 10, "%d", gainFirstDigit);
    float gainFirstDecimalfloat = (gain - gainFirstDigit) * 10.0;
    int gainFirstDecimal = int(gainFirstDecimalfloat);
    str[1] = '.';
    tfp_snprintf(&str[2], 7, "%d", gainFirstDecimal);
    str[9] = 0;
    return str;
  }

  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;

    gain = (float)value * gainFactor /128.0;

    static char str[100];
    int size = tfp_snprintf(str,100, "%s, Gain, %s\n", name, paramToString(id));
    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
  }

  void process(sample_t * samplesToProcess)
  {
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }
    for(uint32_t i=0; i<num_samples; i++){
      outputBuffer[i] = inputBuffer[i] * gain;
    }
  }
};

class ClippingDistortionBlock : public ProcessBlock{
  float clipping_percent;
public:
  ClippingDistortionBlock(const char * name, uint32_t size) :
    ProcessBlock(name, size){
    clipping_percent = 0.5;
  }

  void process(sample_t * samplesToProcess)
  {
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }

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

  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;

    clipping_percent = ((float)value/128.0);
    int clipping_percent_int = (int) (clipping_percent*100.0);
    static char str[100];
    int size = tfp_snprintf(str,100, "%s, Clip(%%), %d\n", name, clipping_percent_int);
    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
  }
};

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


#endif
