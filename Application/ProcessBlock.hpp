#ifndef __PROCESSBLOCK_HPP__
#define __PROCESSBLOCK_HPP__

#include <stdint.h>
#include "SamplingTypes.hpp"
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

public:
  ProcessBlock(void){
    name = "hurr durr";
  }
  ProcessBlock(const char * name_param){
    name = name_param;
  }
  const char * getName(void) {return name;}
  virtual void process(sample_t * samplesToProcess) = 0;
  virtual void MIDIMessageReceived(MIDI_Message_t & msg) = 0;

  virtual void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;(void)value;
  }
  virtual sample_t * getOutputBuffer(void) = 0;
  virtual ~ProcessBlock() {};


  void assignMIDIMessageToParameter(MIDI_Message_t & msg, BlockParamIdentifier_t id){
    midiAssignments[midiAssignmentIndex].msg = msg;
    midiAssignments[midiAssignmentIndex].paramId = id;
    midiAssignmentIndex++;
  }
};

class RealProcessBlock : public ProcessBlock{

protected:
  sample_t * inputBuffer;
  sample_t * outputBuffer;
  uint32_t num_samples;

public:
  RealProcessBlock(const char * name, uint32_t size)
    : ProcessBlock(name){
    num_samples = size;
    inputBuffer = new sample_t[size];
    outputBuffer = new sample_t[size];
  }

  ~RealProcessBlock(){
    delete []inputBuffer;
    delete []outputBuffer;
  }

  sample_t * getOutputBuffer(void){
    return outputBuffer;
  }

  void process(sample_t * samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
      outputBuffer[i] = inputBuffer[i];
    }
    //default function is identity
  }

  void MIDIMessageReceived(MIDI_Message_t & msg){
    for(int i=0; i<midiAssignmentIndex; i++){
      if(midiAssignments[i].msg.type != msg.type)
        continue;
      if(midiAssignments[i].msg.id != msg.id)
        continue;

      setMIDIParameter(midiAssignments[i].paramId, msg.value);
    }
  }

};


class GainBlock : public RealProcessBlock{
  float gain;
  float gainFactor;
public:
  GainBlock(const char * name, uint32_t size) :
    RealProcessBlock(name, size){
    gain = 1.0;
    gainFactor = 8.0;
  }

  void setGainFactor(float factor){
    gainFactor = factor;
  }

  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;

    gain = (float)value * gainFactor /128.0;

    static char str[100];
    int size = tfp_snprintf(str,100, "%s, Gain, %d\n", name, value);
    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_UPDATE, (uint8_t *)str, size);
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

class ClippingDistortionBlock : public RealProcessBlock{
public:
  ClippingDistortionBlock(const char * name, uint32_t size) :
    RealProcessBlock(name, size){
  }

  void process(sample_t * samplesToProcess)
  {
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }

    //clip half way, both ends
    sample_t max = (sample_t) 0x4000/2;
    sample_t min = (sample_t) -(0x4000/2);

    for(uint32_t i=0; i<num_samples; i++){
      if(inputBuffer[i] > max)
        outputBuffer[i] = max;
      else if(inputBuffer[i] < min)
        outputBuffer[i] = min;
      else
        outputBuffer[i] = inputBuffer[i];
    }
  }
};

class MixerBlock : public RealProcessBlock {
public:
  MixerBlock(const char * name, uint32_t size) :
    RealProcessBlock(name, size){
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

    for(uint32_t i=0; i<num_samples; i++){
      outputBuffer[i] = outputBuffer[i] + inputBuffer[i];
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

class DelayBlock : public RealProcessBlock {

  int delayNumSamples;
  DelayBuffer * delayBuffer;

public:
  DelayBlock(const char * name, uint32_t size) :
    RealProcessBlock(name, size){
    delayBuffer = new DelayBuffer(1024*10);//static 10k
    delayNumSamples = 0;
  }

  //for testing
  int getDelayNumSamples(void){return delayNumSamples;}

  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;
    float delayNumSamples_float = ((float)value/128.0) * 100.0 * (1.0/1000.0) * (48000.0);
    delayNumSamples = (int) delayNumSamples_float;

    static char str[100];
    int size = tfp_snprintf(str,100, "%s, Delay, %d\n", name, delayNumSamples);
    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_UPDATE, (uint8_t *)str, size);
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
        + 0.5* delayBuffer->getDelayedSample(delayNumSamples)
        + 0.5* delayBuffer->getDelayedSample(delayNumSamples*2)
        + 0.5* delayBuffer->getDelayedSample(delayNumSamples*4)
        ;
      delayBuffer->insertSample(inputBuffer[i]);
    }
  }
};


#endif
