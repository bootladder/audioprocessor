extern "C" {
#include "AudioProcessor.h"  //Audio_Task uses this interface to pass control to AudioProcessor
}

#include "SamplingTypes.h"
#include "BSP_Audio_Buffer_Interface.h"
#include "ProcessBlock.hpp"
#include "FIRBlock.hpp"
#include "FFTBlock.hpp"
#include "ARMDSPFFTProcessor.hpp"
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

createBlock(FIRBlock                ,fir1        )
createBlock(FIRBlock                ,fir2        )
createBlock(GainBlock               ,gain1       )
createBlock(GainBlock               ,gain2       )
createBlock(GainBlock               ,gain3       )
createBlock(ClippingDistortionBlock ,clipping1   )
createBlock(MixerBlock              ,mixer       )
createBlock(DelayBlock              ,delay       )

static ARMDSPFFTProcessor armDSPFFTProcessor;
static FFTBlock fft1 = FFTBlock("fft1",armDSPFFTProcessor, 4*1024, MY_PROCESSING_BUFFER_SIZE_SAMPLES);

static BlockGraph blockGraph = {
  .start = &gain1,
  .edges = {
  {&gain1, &gain2},
  {&gain1, &gain3},
  {&gain1, &fft1},
  {&gain3, &clipping1},
  {&clipping1, &delay},
  {&delay, &mixer},
  {&gain2, &mixer},
  {0,0}, // null terminator
  },
  .end = &mixer,
};

//static BlockGraph delayTesterGraph = {
//  .start = &gain1,
//  .edges = {
//    {&gain1, &delay},
//  },
//  .end = &delay,
//};
__attribute__ ((unused))
static BlockGraph fftTesterGraph = {
  .start = &gain1,
  .edges = {
    {&gain1, &fft1},
  },
  .end = &gain1,
};


static BlockGraph & active_block_graph = blockGraph;

void AudioProcessor::init(void)
{
  //midi map assigns messages to blocks
  MIDI_Message_t gain1_midi_message = {MIDI_CONTROL_CHANGE,1,1};
  MIDI_Message_t gain2_midi_message = {MIDI_CONTROL_CHANGE,2,1};
  MIDI_Message_t gain3_midi_message = {MIDI_CONTROL_CHANGE,3,1};
  MIDI_Message_t clipping1_midi_message = {MIDI_CONTROL_CHANGE,4,1};
  MIDI_Message_t fir1_midi_message = {MIDI_CONTROL_CHANGE,5,1};
  MIDI_Message_t delay_midi_message = {MIDI_CONTROL_CHANGE,6,1};
  midiMap.addEntry(gain1_midi_message, gain1);
  midiMap.addEntry(gain2_midi_message, gain2);
  midiMap.addEntry(gain3_midi_message, gain3);
  midiMap.addEntry(clipping1_midi_message, clipping1);
  midiMap.addEntry(fir1_midi_message, fir1);
  midiMap.addEntry(delay_midi_message, delay);

  MIDIMessageHandler_RegisterMIDIMap(midiMap);

  //block MIDI assignments assign messages to block params
  gain1.assignMIDIMessageToParameter(gain1_midi_message, PARAM_0);
  gain2.assignMIDIMessageToParameter(gain2_midi_message, PARAM_0);
  gain3.assignMIDIMessageToParameter(gain3_midi_message, PARAM_0);
  clipping1.assignMIDIMessageToParameter(clipping1_midi_message, PARAM_0);
  fir1.assignMIDIMessageToParameter(fir1_midi_message, PARAM_0);
  delay.assignMIDIMessageToParameter(delay_midi_message, PARAM_0);
}

sample_t * AudioProcessor::process(sample_t * sampleBuf)
{
  // call reset() on all of the blocks (particularly to reset the mixers' accumulators)
  mixer.reset();


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


// Called by the monitor task to print out the edge list of the active block graph
extern "C" char * AudioProcessor_GetActiveBlockGraphEdgeListToString(void)
{
  return active_block_graph.toEdgeListJSONString();
}

// Called by the monitor task
extern "C" sample_t * AudioProcessor_GetFFTSpectrum(void)
{
  return fft1.getSpectrum();
}
