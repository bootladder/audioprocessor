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