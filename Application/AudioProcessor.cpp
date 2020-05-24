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
#include "LambdaLFO.hpp"

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
createBlock(IIRBlock                ,iirLFO       )
createBlock(IIRBlock                ,iirLambdaLFO       )

//////////////////////////////////

static ClippingDistortionBlock clipping1("clip1", MY_PROCESSING_BUFFER_SIZE_SAMPLES, ClippingDistortionBlock::CLIPPING_TYPE_SOFT);

//////////////////////////////////

static ARMDSPFFTProcessor armDSPFFTProcessor;
static FFTBlock fft1 = FFTBlock("fft1",armDSPFFTProcessor, 2*1024, MY_PROCESSING_BUFFER_SIZE_SAMPLES);

//////////////////////////////////

sample_t return_constant_440hz(void){return (sample_t)fft1.getSpectrumPeakFreq();}
sample_t return_some_amplitude(void){return (sample_t)fft1.getSpectrumPeakMagnitude();}

OscillatorBlock square1("square1", MY_PROCESSING_BUFFER_SIZE_SAMPLES,
                        OSCILLATOR_SQUARE,
                        return_constant_440hz,
                        return_some_amplitude);


//////////////////////////////////////////////////////////////////////////
// GRAPH

__attribute__ ((unused))
static BlockGraph tempTesterGraph = {
  .start = &gain1,
  .edges = {
    {&gain1, &gain3},
    {&gain3, &clipping1},
    {&clipping1, &iirLambdaLFO},
    {&iirLambdaLFO, &delay},
    {&delay, &mixer},
    {0,0}, // null terminator
  },
  .end = &mixer,
};


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
    {&gain1, &gain3},
    {&gain3, &clipping1},
    {&clipping1, &iir1},
    {&iir1, &delay},
    {&delay, &mixer},
    {0,0}, // null terminator
  },
  .end = &mixer,
};

__attribute__ ((unused))
static BlockGraph graph_ClippingDelay = {
  .start = &gain1,
  .edges = {
    {&gain1, &gain3},
    {&gain3, &clipping1},
    {&clipping1, &delay},
    {&delay, &mixer},
    {0,0}, // null terminator
  },
  .end = &mixer,
};

__attribute__ ((unused))
static BlockGraph graph_ClippingDelayIIRLFO = {
  .start = &gain1,
  .edges = {
    {&gain1, &gain3},
    {&gain3, &clipping1},
    {&clipping1, &delay},
    {&delay, &iirLFO},
    {&iirLFO, &mixer},
    {0,0}, // null terminator
  },
  .end = &mixer,
};


//////////////////////////////////////////////////////////////////////////
// LFOs

LFO lfo1("LFO1");
LFO lfo2("LFO2");
LambdaLFO lambdaLFO1("LambdaLFO1");

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
void dummyLFOTimerFunc(LFO & lfo, int ms){
    lfos[num_lfos] = &lfo;
    num_lfos++;
}

//////////////////////////////////////////////////////////////////////////
// AudioProcessor Class

class AudioProcessor : public ProcessBlock{
  MIDIMap midiMap;
  BlockGraph active_block_graph;// = blockGraph;

public:
  AudioProcessor(const char * name, uint32_t size):
    ProcessBlock(name, size){
      active_block_graph = blockGraph;

    }

  void init(void);
  void process(sample_t * sampleBuf);
  void MIDIHookup(MIDI_Message_t msg, MIDIReceiver &midiReceiver, BlockParamIdentifier_t paramId);


  void setMIDIParameter(BlockParamIdentifier_t id, int value);
  BlockGraph& getActiveBlockGraph(){return active_block_graph;}
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

  //self midi hookup?
  MIDIHookup({MIDI_NOTE_ON,44,1}, *this, PARAM_0);
  MIDIHookup({MIDI_NOTE_ON,45,1}, *this, PARAM_1);
  MIDIHookup({MIDI_NOTE_ON,46,1}, *this, PARAM_2);
  MIDIHookup({MIDI_NOTE_ON,47,1}, *this, PARAM_3);
  MIDIHookup({MIDI_NOTE_ON,48,1}, *this, PARAM_4);
  MIDIHookup({MIDI_NOTE_ON,49,1}, *this, PARAM_5);
  MIDIHookup({MIDI_NOTE_ON,50,1}, *this, PARAM_6);
  MIDIHookup({MIDI_NOTE_ON,51,1}, *this, PARAM_7);

  // LFO

    // LFO LAMBDA HOOKUP
    lambdaLFO1.setLFOFrequencyHz(1);
    lambdaLFO1.setStartTimerMsFunc(dummyLFOTimerFunc);
    lambdaLFO1.startTimerMs(10);
    lambdaLFO1.setMidPoint(800);
    lambdaLFO1.setAmplitude(500);

    auto l = [](int f){iirLambdaLFO.setCutoffFrequency(f);};
    lambdaLFO1.setLambda(l);

    // MIDI IN TO LFO HOOKUP
    MIDIHookup({MIDI_CONTROL_CHANGE,5,1}, lambdaLFO1, PARAM_0);

}

void AudioProcessor::MIDIHookup(MIDI_Message_t msg, MIDIReceiver &midiReceiver, BlockParamIdentifier_t paramId){
  midiMap.addEntry(msg, midiReceiver);
  midiReceiver.assignMIDIMessageToParameter(msg, paramId);
}

void AudioProcessor::process(sample_t * sampleBuf)
{
  // call reset() on all of the blocks (particularly to reset the mixers' accumulators)
  mixer.reset();

  // Update LFOs
  lfos[0]->tickCallback();



  active_block_graph.start->process(sampleBuf);

  auto edges = active_block_graph.edges;
  int i=0;
  while(edges[i].block != 0){
    edges[i].next->process(edges[i].block->getOutputBuffer());
    i++;
  }

  //not copying the output buffer
  outputBuffer = active_block_graph.end->getOutputBuffer();
  //return active_block_graph.end->getOutputBuffer();
}


  void AudioProcessor::setMIDIParameter(BlockParamIdentifier_t id, int value){
    switch(id){
      case PARAM_0: active_block_graph = tempTesterGraph; break;
      case PARAM_1: active_block_graph = blockGraph; break;
      case PARAM_2: active_block_graph = testerGraph; break;
      case PARAM_3: active_block_graph = graph_ClippingDelay; break;
      case PARAM_4: active_block_graph = graph_ClippingDelayIIRLFO; break;
      case PARAM_5: active_block_graph = blockGraph; break;
      case PARAM_6: active_block_graph = blockGraph; break;
      case PARAM_7: active_block_graph = blockGraph; break;
    }
  }


/////////////////////////////////////////////////////////////////////////
// C Interface

// The C interface needs static functions to call
// so we have this global instance of AudioProcessor here.
AudioProcessor audioProcessor("procesor", MY_PROCESSING_BUFFER_SIZE_SAMPLES);



// This is called by the FreeRTOS Audio Task, every time the DMA receive buf fills up.
// Takes int16_t samples, processes, returns int16_t samples.
// The processor takes sample_t's, currently defined to be floats

// The AudioProcessor MUST be initialized!!
// Make a call to AudioProcessor_Init() !!!
static sample_t inputSampleBuffer[MY_PROCESSING_BUFFER_SIZE_SAMPLES];  //max size
static int16_t outputInt16Buffer[MY_PROCESSING_BUFFER_SIZE_SAMPLES];

extern "C"
int16_t *
AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{

  for(uint32_t i=0; i<num_samples; i++){
    inputSampleBuffer[i] = (sample_t)sampleBuf[i];
  }

  audioProcessor.process(inputSampleBuffer);
  sample_t * out = audioProcessor.getOutputBuffer();

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
  return audioProcessor.getActiveBlockGraph().toEdgeListJSONString();
}

// Called by the monitor task
extern "C" sample_t * AudioProcessor_GetFFTSpectrum(void)
{
  return fft1.getSpectrum();
}
