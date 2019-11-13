#ifndef __PROCESSBLOCK_HPP__
#define __PROCESSBLOCK_HPP__

#include <stdint.h>
#include "SamplingTypes.hpp"
#include "BlockState.hpp"
#include "ProcessBlockFunctions_FIRFilters.hpp"
#include "ProcessBlockFunctions.hpp"
#include "MIDI_Message.h"

extern "C"{
#include "SerialLogger.h"
#include "tinyprintf.h"
}


typedef void (* ProcessBlockFunctionPointer)(BlockState *, sample_t *, sample_t *, uint32_t);

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
  ProcessBlockFunctionPointer processFunc;
  sample_t * inputBuffer;
  sample_t * outputBuffer;
  uint32_t num_samples;

  //BlockState * blockState;
public:
  RealProcessBlock(const char * name, ProcessBlockFunctionPointer func, uint32_t size)
    : ProcessBlock(name){
    processFunc = func;
    num_samples = size;
    inputBuffer = new sample_t[size];
    outputBuffer = new sample_t[size];

    //blockState = new BlockState();
  }
  RealProcessBlock(ProcessBlockFunctionPointer func, uint32_t size){
    processFunc = func;
    num_samples = size;
    inputBuffer = new sample_t[size];
    outputBuffer = new sample_t[size];

    //blockState = new BlockState();
  }

  ~RealProcessBlock(){
    delete []inputBuffer;
    delete []outputBuffer;
  }

  sample_t * getOutputBuffer(void){
    return outputBuffer;
  }

  virtual void setMIDIParameter(BlockParamIdentifier_t id, int value){
    //    blockState -> setParam(id, value);
    //    static char str[100];
    //    int size = tfp_snprintf(str,100, "%s, Param, %d\n", name, value);
    //    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_UPDATE, (uint8_t *)str, size);
  }

  //BlockState * getBlockState(void){return blockState;}

  void process(sample_t * samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
      outputBuffer[i] = inputBuffer[i];
    }

    //default function is identity
    //processFunc(blockState, inputBuffer, outputBuffer, num_samples);
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


// delete this
#include <iostream>
using namespace std;

class GainBlock : public RealProcessBlock{
  float gain;
  float gainFactor;
public:
  GainBlock(const char * name, uint32_t size) :
    RealProcessBlock(name, ProcessBlockFunctions_GainParameterized, size){
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
    RealProcessBlock(name, ProcessBlockFunctions_ClippingDistortion, size){
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
    RealProcessBlock(name, ProcessBlockFunctions_Mixer, size){
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


class DelayBlock : public RealProcessBlock {

  int delayNumSamples;
  DelayBuffer * delayBuffer;

public:
  DelayBlock(const char * name, uint32_t size) :
    RealProcessBlock(name, ProcessBlockFunctions_Identity, size){
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
