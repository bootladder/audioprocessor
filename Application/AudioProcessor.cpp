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

#define createBlock(blockClass, name) static blockClass name(#name,MY_PROCESSING_BUFFER_SIZE_SAMPLES);

createBlock(FIRBlock                ,firBlock1        )
createBlock(FIRBlock                ,firBlock2        )
createBlock(GainBlock               ,gainBlock1       )
createBlock(GainBlock               ,gainBlock2       )
createBlock(GainBlock               ,gainBlock3       )
createBlock(ClippingDistortionBlock ,clippingBlock1   )
createBlock(MixerBlock              ,mixerBlock       )
createBlock(DelayBlock              ,delayBlock       )

static BlockGraph blockGraph = {
  .start = &gainBlock1,
  .edges = {
    {&gainBlock1, &gainBlock2},
    {&gainBlock1, &gainBlock3},
    {&gainBlock3, &clippingBlock1},
    {&clippingBlock1, &delayBlock},
    {&delayBlock, &mixerBlock},
    {&gainBlock2, &mixerBlock},

    {&mixerBlock, &firBlock1},
    {&firBlock1, &firBlock2},
    {0,0}, // null terminator
  },
  .end = &firBlock2,
};

static BlockGraph delayTesterGraph = {
  .start = &gainBlock1,
  .edges = {
    {&gainBlock1, &delayBlock},
  },
  .end = &delayBlock,
};


static BlockGraph & active_block_graph = blockGraph;

void AudioProcessor::init(void)
{
  //midi map assigns messages to blocks
  MIDI_Message_t gainBlock1_midi_message = {MIDI_CONTROL_CHANGE,1,1};
  MIDI_Message_t gainBlock2_midi_message = {MIDI_CONTROL_CHANGE,2,1};
  MIDI_Message_t gainBlock3_midi_message = {MIDI_CONTROL_CHANGE,3,1};
  MIDI_Message_t firBlock1_midi_message = {MIDI_CONTROL_CHANGE,5,1};
  MIDI_Message_t delayBlock_midi_message = {MIDI_CONTROL_CHANGE,6,1};
  midiMap.addEntry(gainBlock1_midi_message, gainBlock1);
  midiMap.addEntry(gainBlock2_midi_message, gainBlock2);
  midiMap.addEntry(gainBlock3_midi_message, gainBlock3);
  midiMap.addEntry(firBlock1_midi_message, firBlock1);
  midiMap.addEntry(delayBlock_midi_message, delayBlock);

  MIDIMessageHandler_RegisterMIDIMap(midiMap);

  //block MIDI assignments assign messages to block params
  gainBlock1.assignMIDIMessageToParameter(gainBlock1_midi_message, PARAM_0);
  gainBlock2.assignMIDIMessageToParameter(gainBlock2_midi_message, PARAM_0);
  gainBlock3.assignMIDIMessageToParameter(gainBlock3_midi_message, PARAM_0);
  firBlock1.assignMIDIMessageToParameter(firBlock1_midi_message, PARAM_0);
  delayBlock.assignMIDIMessageToParameter(delayBlock_midi_message, PARAM_0);


  // DURRR these will block the queue because they run too early
  //initial block params, these are MIDI values, ie 0 to 127
  //gainBlock1.setMIDIParameter(PARAM_0, 16);
  //gainBlock2.setMIDIParameter(PARAM_0, 16);
  //gainBlock3.setMIDIParameter(PARAM_0, 16);
}

sample_t * AudioProcessor::process(sample_t * sampleBuf)
{
  // call reset() on all of the blocks (particularly to reset the mixers' accumulators)
  mixerBlock.reset();


  active_block_graph.start->process(sampleBuf);

  auto edges = active_block_graph.edges;
  int i=0;
  while(edges[i].block != 0){
    edges[i].next->process(edges[i].block->getOutputBuffer());
    i++;
  }

  return active_block_graph.end->getOutputBuffer();
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

