#include <iostream>
using namespace std;
#include "gtest/gtest.h"

#include "ProcessBlock.hpp"
#include "ProcessBlockFunctions.hpp"

#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];

static void init_testBuf_with_staircase(void){
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    testBuf[i] = i;
  }
}

TEST(ProcessBlock, identity)
{
  RealProcessBlock block("name", ProcessBlockFunctions_Identity, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.process(testBuf);

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i, testBuf[i]);
  }
}

TEST(ProcessBlock, gain2x)
{
  RealProcessBlock block("name", ProcessBlockFunctions_Gain2X, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*2, out[i]);
  }
}

TEST(ProcessBlock, gainParameterized)
{
  RealProcessBlock block("name", ProcessBlockFunctions_GainParameterized, NUM_SAMPLES);

  init_testBuf_with_staircase();

  // the gain formula is 8.0 * value / 128.0
  block.setMIDIParameter(PARAM_0, 128);
  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*8, out[i]);
  }
}

TEST(ProcessBlock, attenuation)
{
  RealProcessBlock block("name", ProcessBlockFunctions_Attenuation, NUM_SAMPLES);

  init_testBuf_with_staircase();

  // the attenuation formula is 2.0 * value / 128.0
  block.setMIDIParameter(PARAM_0, 64);
  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*1, out[i]);
  }
}

TEST(ProcessBlock, clippingdistortion_DefaultSettings_ClipsAtHalfAmplitudePositiveAndNegative)
{
  RealProcessBlock block("name", ProcessBlockFunctions_ClippingDistortion, NUM_SAMPLES);

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    testBuf[i] = -0x8000 + (i*0x10000)/NUM_SAMPLES;
  }

  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_LE(out[i], 0x8000/2);
    ASSERT_GE(out[i], -(0x8000/2));
  }
}

TEST(ProcessBlock, Mixer_2_Inputs_Mixes)
{
  MixerBlock mixerBlock("name", NUM_SAMPLES);

  sample_t inputBuf1[NUM_SAMPLES];
  sample_t inputBuf2[NUM_SAMPLES];

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    inputBuf1[i] = 2;
    inputBuf2[i] = 3;
  }

  mixerBlock.process(inputBuf1);
  mixerBlock.process(inputBuf2);

  sample_t * out = mixerBlock.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ(out[i], 5);
  }
}

//need to clear the output buffer between subsequent mixes
//for now, add a reset() method
TEST(ProcessBlock, Mixer_CalledTwice_Works)
{
  MixerBlock mixerBlock("name", NUM_SAMPLES);

  sample_t inputBuf1[NUM_SAMPLES];
  sample_t inputBuf2[NUM_SAMPLES];
  sample_t * out;

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    inputBuf1[i] = 2;
    inputBuf2[i] = 3;
  }

  mixerBlock.process(inputBuf1);
  mixerBlock.process(inputBuf2);

  out = mixerBlock.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ(out[i], 5);
  }

  //reset the mixer block
  mixerBlock.reset();
  // do the same thing again

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    inputBuf1[i] = 2;
    inputBuf2[i] = 3;
  }

  mixerBlock.process(inputBuf1);
  mixerBlock.process(inputBuf2);

  out = mixerBlock.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ(out[i], 5);
  }
}

TEST(ProcessBlockDelay, SetMIDIParameter_SetsDelayNumberOfSamples)
{
  // Set Delay parameter
  // Formula should be : (X/128) * 100ms
  // so 64 ==> 50ms
  // Actual delay in samples D = (X/128) * 100ms * (1sec/1000ms) * (SAMPLERATE samples/1sec)

  float delayNumSamples_float = (64.0/128.0) * 100.0 * (1.0/1000.0) * (48000.0);
  int delayNumSamples = (int) delayNumSamples_float;

  DelayBlock delayBlock("name", NUM_SAMPLES);
  delayBlock.setMIDIParameter(PARAM_0, 64);

  ASSERT_EQ(delayNumSamples, delayBlock.getDelayNumSamples());
}

TEST(DelayBuffer, FillUpHalfWay)
{
  #define TEST_DELAY_BUFFER_SIZE 16
  DelayBuffer delayBuffer(TEST_DELAY_BUFFER_SIZE);

  //fill up half way
  for(int i=0; i<TEST_DELAY_BUFFER_SIZE/2; i++){
    sample_t samp = i;
    delayBuffer.insertSample(samp);
  }

  ASSERT_EQ(delayBuffer.getDelayedSample(0), (TEST_DELAY_BUFFER_SIZE / 2) - 1);
  ASSERT_EQ(delayBuffer.getDelayedSample(1), (TEST_DELAY_BUFFER_SIZE / 2) - 2);
  ASSERT_EQ(delayBuffer.getDelayedSample(2), (TEST_DELAY_BUFFER_SIZE / 2) - 3);
  ASSERT_EQ(delayBuffer.getDelayedSample(3), (TEST_DELAY_BUFFER_SIZE / 2) - 4);
  ASSERT_EQ(delayBuffer.getDelayedSample(4), (TEST_DELAY_BUFFER_SIZE / 2) - 5);
}
TEST(DelayBuffer, FillUpFull)
{
  #define TEST_DELAY_BUFFER_SIZE 16
  DelayBuffer delayBuffer(TEST_DELAY_BUFFER_SIZE);

  //fill up whole way
  for(int i=0; i<TEST_DELAY_BUFFER_SIZE; i++){
    sample_t samp = i;
    delayBuffer.insertSample(samp);
  }

  ASSERT_EQ(delayBuffer.getDelayedSample(0), (TEST_DELAY_BUFFER_SIZE) - 1);
  ASSERT_EQ(delayBuffer.getDelayedSample(1), (TEST_DELAY_BUFFER_SIZE) - 2);
  ASSERT_EQ(delayBuffer.getDelayedSample(2), (TEST_DELAY_BUFFER_SIZE) - 3);
  ASSERT_EQ(delayBuffer.getDelayedSample(3), (TEST_DELAY_BUFFER_SIZE) - 4);
  ASSERT_EQ(delayBuffer.getDelayedSample(4), (TEST_DELAY_BUFFER_SIZE) - 5);

}

TEST(DelayBuffer, FillUpFullPlusACoupleMore)
{
  #define TEST_DELAY_BUFFER_SIZE 16
  DelayBuffer delayBuffer(TEST_DELAY_BUFFER_SIZE);

  //fill up whole way
  for(int i=0; i<TEST_DELAY_BUFFER_SIZE; i++){
    sample_t samp = i;
    delayBuffer.insertSample(samp);
  }

  delayBuffer.insertSample((sample_t) 370);
  delayBuffer.insertSample((sample_t) 371);
  delayBuffer.insertSample((sample_t) 372);

  ASSERT_EQ(delayBuffer.getDelayedSample(0), 372);
  ASSERT_EQ(delayBuffer.getDelayedSample(1), 371);
  ASSERT_EQ(delayBuffer.getDelayedSample(2), 370);
}

TEST(DelayBuffer, FillUpLargeAmount)
{
  #define TEST_DELAY_BUFFER_SIZE 16
  DelayBuffer delayBuffer(TEST_DELAY_BUFFER_SIZE);

  //fill up a large amoutn
  int largeNumber = (TEST_DELAY_BUFFER_SIZE * 7) + 376;
  for(int i=0; i < largeNumber ; i++){
    sample_t samp = i;
    delayBuffer.insertSample(samp);
  }

  ASSERT_EQ(delayBuffer.getDelayedSample(0), largeNumber - 1);
  ASSERT_EQ(delayBuffer.getDelayedSample(1), largeNumber - 2);
  ASSERT_EQ(delayBuffer.getDelayedSample(2), largeNumber - 3);
  ASSERT_EQ(delayBuffer.getDelayedSample(3), largeNumber - 4);
  ASSERT_EQ(delayBuffer.getDelayedSample(4), largeNumber - 5);
}

TEST(ProcessBlockDelay, DelaysCorrectNumberOfSamples)
{
  DelayBlock delayBlock("name", NUM_SAMPLES);

  //output is equal to the input plus the delayed input
  static sample_t inputBuf[NUM_SAMPLES*8];

  for(int i=0; i<NUM_SAMPLES * 8; i++){
    inputBuf[i] = (sample_t)i;
  }

  delayBlock.setMIDIParameter(PARAM_0, 64);
  int delayNumSamples = delayBlock.getDelayNumSamples();

  cout << "NUMBER OF DELAY SAMPLES: " << delayNumSamples << endl;


  delayBlock.process(inputBuf + (NUM_SAMPLES * 0) );
  delayBlock.process(inputBuf + (NUM_SAMPLES * 1) );
  delayBlock.process(inputBuf + (NUM_SAMPLES * 2) );
  delayBlock.process(inputBuf + (NUM_SAMPLES * 3) );
  delayBlock.process(inputBuf + (NUM_SAMPLES * 4) );
  delayBlock.process(inputBuf + (NUM_SAMPLES * 5) );
  delayBlock.process(inputBuf + (NUM_SAMPLES * 6) );
  delayBlock.process(inputBuf + (NUM_SAMPLES * 7) );

  sample_t * out = delayBlock.getOutputBuffer();

  sample_t * lastPartOfInputBuf = &inputBuf[NUM_SAMPLES*7];

  for(int i=0; i<NUM_SAMPLES; i++){
    //cout << "i : " << i << " OUT[i] : " << out[i] << " lastPartOfInputBuf[i] : " << lastPartOfInputBuf[i] << endl;;
    ASSERT_EQ(out[i],  lastPartOfInputBuf[i] + lastPartOfInputBuf[i - delayNumSamples - 1]);
  }
}

TEST(ProcessBlock_MIDI, MIDIMessageReceived_PassesTheValue)
{
  RealProcessBlock block("name", ProcessBlockFunctions_GainParameterized, NUM_SAMPLES);

  MIDI_Message_t msg;
  msg.type = 0x9;
  msg.id = 77;
  msg.value = 99;

  block.assignMIDIMessageToParameter(msg, PARAM_0);

  block.MIDIMessageReceived(msg);

  BlockState * state = block.getBlockState();

  ASSERT_EQ(state->getParam(PARAM_0), 99);
}

TEST(ProcessBlock_MIDI, MIDIMessageReceived_TwoMessagesAssigned_PassesTheValue)
{
  RealProcessBlock block("name", ProcessBlockFunctions_GainParameterized, NUM_SAMPLES);

  MIDI_Message_t msg1;
  msg1.type = 0x9;
  msg1.id = 77;
  msg1.value = 99;

  MIDI_Message_t msg2;
  msg2.type = 0x9;
  msg2.id = 88;
  msg2.value = 33;

  block.assignMIDIMessageToParameter(msg1, PARAM_0);
  block.assignMIDIMessageToParameter(msg2, PARAM_1);

  block.MIDIMessageReceived(msg1);
  block.MIDIMessageReceived(msg2);

  BlockState * state = block.getBlockState();

  ASSERT_EQ(state->getParam(PARAM_0), 99);
  ASSERT_EQ(state->getParam(PARAM_1), 33);
}
