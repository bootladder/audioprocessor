#ifndef __PROCESSBLOCK_HPP__
#define __PROCESSBLOCK_HPP__

#include <stdint.h>
#include "SamplingTypes.h"
#include "MIDI_Message.h"

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

  //This is virtual so the mock can check that it is called by MIDIMessageHandler_Handle
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


#endif
