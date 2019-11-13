#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

extern "C"{
#include "MIDIMessageHandler.h"
}
#include "MIDIMessageHandler.hpp"
#include "MIDI_Message.h"
#include "MIDIMap.hpp"
#include "ProcessBlock.hpp"
#include "ProcessBlockFunctions.hpp"

#define TEST_NUM_SAMPLES 1024

class MockProcessBlock : public ProcessBlock{
public:
  MockProcessBlock(ProcessBlockFunctionPointer func, uint32_t size){(void)func;(void)size;}
  MOCK_METHOD(void, MIDIMessageReceived, (MIDI_Message_t & msg), (override));
  MOCK_METHOD(void, process, (sample_t * samplesToProcess), (override));
  MOCK_METHOD(sample_t*, getOutputBuffer, (), (override));
};

TEST(MIDIMessageHandler, MapInitialized_HandleCalled_BlockReceivesMessage)
{
  MIDI_Message_t msg;
  msg.type = 0x9;
  msg.id = 77;
  msg.value = 99;

  MockProcessBlock mockProcessBlock(ProcessBlockFunctions_GainParameterized, TEST_NUM_SAMPLES);

  MIDIMap midiMap;
  midiMap.addEntry(msg, mockProcessBlock);
  MIDIMessageHandler_RegisterMIDIMap(midiMap);

  EXPECT_CALL(mockProcessBlock, MIDIMessageReceived(_));
  MIDIMessageHandler_Handle(msg);
}

TEST(MIDIMessageHandler, MapNotInitialized_DoesNotSegfault)
{
  MIDI_Message_t msg;
  msg.type = 0x9;
  msg.id = 77;
  msg.value = 99;

  MIDIMap midiMap;  //need to reinitialize midiMap or else it segfaults anyway
  MIDIMessageHandler_RegisterMIDIMap(midiMap);

  MIDIMessageHandler_Handle(msg);
}


TEST(ProcessBlock_MIDI, MIDIMessageReceived_PassesTheValue)
{
  //RealProcessBlock block("name", ProcessBlockFunctions_GainParameterized, NUM_SAMPLES);
  MockProcessBlock block(ProcessBlockFunctions_GainParameterized, TEST_NUM_SAMPLES);

  MIDI_Message_t msg;
  msg.type = 0x9;
  msg.id = 77;
  msg.value = 99;

  block.assignMIDIMessageToParameter(msg, PARAM_0);

  block.MIDIMessageReceived(msg);

  //replace with mock expect
  //BlockState * state = block.getBlockState();

  //ASSERT_EQ(state->getParam(PARAM_0), 99);
}

TEST(ProcessBlock_MIDI, MIDIMessageReceived_TwoMessagesAssigned_PassesTheValue)
{
  RealProcessBlock block("name", ProcessBlockFunctions_GainParameterized, TEST_NUM_SAMPLES);

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

  //  BlockState * state = block.getBlockState();
  //
  //  ASSERT_EQ(state->getParam(PARAM_0), 99);
  //  ASSERT_EQ(state->getParam(PARAM_1), 33);
}
