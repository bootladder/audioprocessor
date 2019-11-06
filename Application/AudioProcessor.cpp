extern "C" {
#include "AudioProcessor.h"  //Audio_Task uses this interface to pass control to AudioProcessor
}

#include "SamplingTypes.hpp"
#include "BSP_Audio_Buffer_Interface.h"
#include "ProcessBlock.hpp"
#include "ProcessBlockFunctions.hpp"
#include "ProcessBlockFunctions_FIRFilters.hpp"
#include "FIRBlock.hpp"
#include "MIDIMap.hpp"
#include "MIDIMessageHandler.hpp"
#include "BlockGraph.hpp"

class AudioProcessor{
  MIDIMap midiMap;
public:
  void init(void);
  sample_t * process(sample_t * sampleBuf);
};

AudioProcessor audioProcessor;


// Currently, everything is configured statically and manually.
// Eventually there will be some configuration format
// That will be parsed into the Processing Graph and the MIDI Map.

static FIRBlock firBlock1(MY_PROCESSING_BUFFER_SIZE_SAMPLES, 5000);
static FIRBlock firBlock2(MY_PROCESSING_BUFFER_SIZE_SAMPLES, 20000);

static GainBlock gainBlock1(MY_PROCESSING_BUFFER_SIZE_SAMPLES);
static GainBlock gainBlock2(MY_PROCESSING_BUFFER_SIZE_SAMPLES);

static ClippingDistortionBlock clippingBlock1(MY_PROCESSING_BUFFER_SIZE_SAMPLES);

static BlockGraph blockGraph = {
  .start = &gainBlock1,
  .edges = {
    {&gainBlock1, &clippingBlock1},
    {&clippingBlock1, &gainBlock2},
    {&gainBlock2, &firBlock1},
    {0,0}, // null terminator
  },
  .end = &firBlock1,
};

void AudioProcessor::init(void)
{
  //midi map
  MIDI_Message_t gainBlock1_midi_message = {MIDI_CONTROL_CHANGE,1,1};
  MIDI_Message_t gainBlock2_midi_message = {MIDI_CONTROL_CHANGE,2,1};
  MIDI_Message_t firBlock1_midi_message = {MIDI_CONTROL_CHANGE,5,1};
  midiMap.addEntry(gainBlock1_midi_message, gainBlock1);
  midiMap.addEntry(gainBlock2_midi_message, gainBlock2);
  midiMap.addEntry(firBlock1_midi_message, firBlock1);

  MIDIMessageHandler_RegisterMIDIMap(midiMap);

  //block MIDI assignments
  gainBlock1.assignMIDIMessageToParameter(gainBlock1_midi_message, PARAM_0);
  gainBlock2.assignMIDIMessageToParameter(gainBlock2_midi_message, PARAM_0);
  firBlock1.assignMIDIMessageToParameter(firBlock1_midi_message, PARAM_0);

  //initial block params
  gainBlock1.setMIDIParameter(PARAM_0, 1);
  gainBlock2.setMIDIParameter(PARAM_0, 1);
}

sample_t * AudioProcessor::process(sample_t * sampleBuf)
{
  blockGraph.start->process(sampleBuf);

  auto edges = blockGraph.edges;
  int i=0;
  while(edges[i].block != 0){
    edges[i].next->process(edges[i].block->getOutputBuffer());
    i++;
  }

  return blockGraph.end->getOutputBuffer();
}

///////////////////////////////////////////////////////////////////////////////////

// This is called by the FreeRTOS Audio Task, every time the DMA receive buf fills up.
// Takes int16_t samples, processes, returns int16_t samples.
// The processor takes sample_t's, currently defined to be floats

// The AudioProcessor MUST be initialized!!
// Make a call to AudioProcessor_Init() !!!
extern "C"
int16_t *
AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  static sample_t inputSampleBuffer[MY_PROCESSING_BUFFER_SIZE_SAMPLES];  //max size
  static int16_t outputInt16Buffer[MY_PROCESSING_BUFFER_SIZE_SAMPLES];

  for(uint32_t i=0; i<num_samples; i++){
    inputSampleBuffer[i] = (sample_t)sampleBuf[i];
  }

  sample_t * out = audioProcessor.process(inputSampleBuffer);

  for(uint32_t i=0; i<num_samples; i++){
    outputInt16Buffer[i] = (int16_t)out[i];
  }

  return outputInt16Buffer;
}


// This is called in the BSP_Audio_Task init.
// Could do a constructor but at this point I want the init to execute later.
// For example, the UARTs won't be initialized when the global constructors run.
extern "C" void AudioProcessor_Init(void)
{
  audioProcessor.init();
}

