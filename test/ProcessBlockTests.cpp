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
  RealProcessBlock block(ProcessBlockFunctions_Identity, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.process(testBuf);

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i, testBuf[i]);
  }
}

TEST(ProcessBlock, gain2x)
{
  RealProcessBlock block(ProcessBlockFunctions_Gain2X, NUM_SAMPLES);

  init_testBuf_with_staircase();

  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*2, out[i]);
  }
}

TEST(ProcessBlock, gainParameterized)
{
  RealProcessBlock block(ProcessBlockFunctions_GainParameterized, NUM_SAMPLES);

  init_testBuf_with_staircase();

  // the gain formula is 8.0 * value / 128.0
  block.setParam(PARAM_0, 128);
  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*8, out[i]);
  }
}

TEST(ProcessBlock, attenuation)
{
  RealProcessBlock block(ProcessBlockFunctions_Attenuation, NUM_SAMPLES);

  init_testBuf_with_staircase();

  // the attenuation formula is 2.0 * value / 128.0
  block.setParam(PARAM_0, 64);
  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_EQ((sample_t)i*1, out[i]);
  }
}

TEST(ProcessBlock, clippingdistortion_DefaultSettings_ClipsAtHalfAmplitudePositiveAndNegative)
{
  RealProcessBlock block(ProcessBlockFunctions_ClippingDistortion, NUM_SAMPLES);

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

TEST(ProcessBlock, MIDIMessageReceived_PassesTheValue)
{
  RealProcessBlock block(ProcessBlockFunctions_GainParameterized, NUM_SAMPLES);

  MIDI_Message_t msg;
  msg.type = 0x9;
  msg.id = 77;
  msg.value = 99;

  block.assignMIDIMessageToParameter(msg, PARAM_0);

  block.MIDIMessageReceived(msg);

  BlockState * state = block.getBlockState();

  ASSERT_EQ(state->getParam(PARAM_0), 99);
}

TEST(ProcessBlock, MIDIMessageReceived_TwoMessagesAssigned_PassesTheValue)
{
  RealProcessBlock block(ProcessBlockFunctions_GainParameterized, NUM_SAMPLES);

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
