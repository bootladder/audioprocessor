#ifndef __PROCESSBLOCK_HPP__
#define __PROCESSBLOCK_HPP__

#include <stdint.h>
#include "SamplingTypes.h"
#include "MIDI_Message.hpp"
#include "MIDIReceiver.hpp"



class ProcessBlock  : public MIDIReceiver {

protected:
  const char * name;
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
  virtual void process(const sample_t *samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
      outputBuffer[i] = inputBuffer[i];
    }
  }


};


#endif
