extern "C" {
#include "AudioProcessor.h"  //Audio_Task uses this interface to pass control to AudioProcessor
}

#include "SamplingTypes.h"
#include "BSP_Audio_Buffer_Interface.h"
#include "ProcessBlock.hpp"
#include "OscillatorBlock.hpp"
#include "FFTBlock.hpp"
#include "ARMDSPFFTProcessor.hpp"
#include "MIDIMap.hpp"
#include "MIDIMessageHandler.hpp"
#include "BlockGraph.hpp"
#include "SuperSimpleProcessBlocks.hpp"
#include "ClippingDistortionBlock.hpp"
#include "DelayBlock.hpp"
#include "MixerBlock.hpp"
#include "IIRBlock.hpp"
#include "LFO.hpp"

// Currently, everything is configured statically and manually.
// Eventually there will be some configuration format
// That will be parsed into the Processing Graph and the MIDI Map.

//////////////////////////////////////////////////////////////////////////
// BLOCKS

#define createBlock(blockClass, name) static blockClass name(#name,MY_PROCESSING_BUFFER_SIZE_SAMPLES);

createBlock(GainBlock               ,gain1       )
createBlock(GainBlock               ,gain2       )
createBlock(GainBlock               ,gain3       )
createBlock(MixerBlock              ,mixer       )
createBlock(DelayBlock              ,delay       )
createBlock(IIRBlock                ,iir1       )

//////////////////////////////////

static ClippingDistortionBlock clipping1("clip1", MY_PROCESSING_BUFFER_SIZE_SAMPLES, ClippingDistortionBlock::CLIPPING_TYPE_SOFT);

//////////////////////////////////

static ARMDSPFFTProcessor armDSPFFTProcessor;
static FFTBlock fft1 = FFTBlock("fft1",armDSPFFTProcessor, 2*1024, MY_PROCESSING_BUFFER_SIZE_SAMPLES);

//////////////////////////////////

float return_constant_440hz(void){return (float)fft1.getSpectrumPeakFreq();}
float return_some_amplitude(void){return (float)fft1.getSpectrumPeakMagnitude();}

OscillatorBlock square1("square1", MY_PROCESSING_BUFFER_SIZE_SAMPLES,
                        OSCILLATOR_SQUARE,
                        return_constant_440hz,
                        return_some_amplitude);


//////////////////////////////////////////////////////////////////////////
// GRAPH

__attribute__ ((unused))
static BlockGraph blockGraph = {
  .start = &gain1,
  .edges = {
  {&gain1, &square1},
  {&square1, &gain2},
  {&gain1, &gain3},
  {&gain1, &fft1},
  {&gain3, &clipping1},
  {&clipping1, &iir1},
  {&iir1, &delay},
  {&delay, &mixer},
  {&gain2, &mixer},
  {0,0}, // null terminator
  },
  .end = &mixer,
};

__attribute__ ((unused))
static BlockGraph testerGraph = {
  .start = &gain1,
  .edges = {
    {&gain1, &gain2},
    {&gain2, &clipping1},
    {&clipping1, &iir1},
    {&iir1, &delay},
    {&delay, &mixer},
    {0,0}, // null terminator
  },
  .end = &mixer,
};


static BlockGraph & active_block_graph = blockGraph;

//////////////////////////////////////////////////////////////////////////
// LFOs

LFO lfo1("LFO1");
LFO lfo2("LFO2");

extern "C"{
#include "FreeRTOS.h"
#include "timers.h"
}


int num_lfos = 0;
LFO * lfos[10];

//this sucks
void vTimerCallback( TimerHandle_t xTimer ){
  auto id = ( uint32_t ) pvTimerGetTimerID( xTimer );
  lfos[id]->tickCallback();
}

// MOVE THIS SOMEWHERE ELSE
void freertosLFOTimerFunc(LFO & lfo, int ms){

  TimerHandle_t handle = xTimerCreate
  (
    "Timer",
    /* The timer period in ticks, must be
    greater than 0. */
    ms,
    /* The timers will auto-reload themselves
    when they expire. */
    pdTRUE,
    /* Timer ID */
    ( void * ) num_lfos,
    /* Each timer calls the same callback when
    it expires. */
    vTimerCallback
  );

  if( xTimerStart( handle, 0 ) != pdPASS )
             {
                 /* The timer could not be set into the Active
                 state. */
             }


    lfos[num_lfos] = &lfo;
    num_lfos++;
}

//////////////////////////////////////////////////////////////////////////
// AudioProcessor Class

class AudioProcessor{
  MIDIMap midiMap;
public:
  void init(void);
  sample_t * process(sample_t * sampleBuf);
  void MIDIHookup(MIDI_Message_t msg, ProcessBlock &block, BlockParamIdentifier_t paramId);
};


void AudioProcessor::init(void)
{
  MIDIHookup({MIDI_CONTROL_CHANGE,1,1}, gain1, PARAM_0);
  MIDIHookup({MIDI_CONTROL_CHANGE,2,1}, gain2, PARAM_0);
  MIDIHookup({MIDI_CONTROL_CHANGE,3,1}, gain3, PARAM_0);
  MIDIHookup({MIDI_CONTROL_CHANGE,4,1}, clipping1, PARAM_0);
  MIDIHookup({MIDI_CONTROL_CHANGE,6,1}, delay, PARAM_0);
  MIDIHookup({MIDI_CONTROL_CHANGE,7,1}, iir1, PARAM_0);
  MIDIHookup({MIDI_CONTROL_CHANGE,20,1}, iir1, PARAM_0);
  MIDIHookup({MIDI_CONTROL_CHANGE,8,1}, iir1, PARAM_1);

  MIDIHookup({MIDI_CONTROL_CHANGE,9,1}, iir1, PARAM_0);

  MIDIHookup({MIDI_NOTE_ON,2,1}, square1, PARAM_0);
  MIDIHookup({MIDI_NOTE_OFF,2,1}, square1, PARAM_1);

  MIDIMessageHandler_RegisterMIDIMap(midiMap);

  MIDIHookup({MIDI_CONTROL_CHANGE,30,1}, delay, PARAM_0);
  MIDIHookup({MIDI_CONTROL_CHANGE,90,1}, gain3, PARAM_0);


  //lfo1.setLFOFrequencyHz(1);
  //lfo1.setMIDIMessage({MIDI_CONTROL_CHANGE,90,1});
  //lfo1.setStartTimerMsFunc(freertosLFOTimerFunc);
  //lfo1.startTimerMs(10);

  //lfo2.setLFOFrequencyHz(1);
  //lfo2.setMIDIMessage({MIDI_CONTROL_CHANGE,30,1});
  //lfo2.setStartTimerMsFunc(freertosLFOTimerFunc);
  //lfo2.startTimerMs(10);
}

void AudioProcessor::MIDIHookup(MIDI_Message_t msg, ProcessBlock &block, BlockParamIdentifier_t paramId){
  midiMap.addEntry(msg, block);
  block.assignMIDIMessageToParameter(msg, paramId);
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



/////////////////////////////////////////////////////////////////////////
// C Interface

// The C interface needs static functions to call
// so we have this global instance of AudioProcessor here.
AudioProcessor audioProcessor;



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
