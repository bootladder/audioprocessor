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

#define TEST_NUM_SAMPLES 1024

class MockProcessBlock : public ProcessBlock{
public:
  MockProcessBlock(uint32_t size): ProcessBlock("name", size) {(void)size;}
  MOCK_METHOD(void, MIDIMessageReceived, (MIDI_Message_t & msg), (override));
  MOCK_METHOD(void, setMIDIParameter, (BlockParamIdentifier_t id, int value));
  MOCK_METHOD(void, process, (sample_t * samplesToProcess), (override));
};

TEST(MIDIMessageHandler, MapInitialized_HandleCalled_BlockReceivesMessage)
{
  MIDI_Message_t msg;
  msg.type = 0x9;
  msg.id = 77;
  msg.value = 99;

  MockProcessBlock mockProcessBlock(TEST_NUM_SAMPLES);

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


TEST(MIDIMap, AddEntry_and_Lookup)
{
  MIDI_Message_t msg;
  msg.type = 0x9;
  msg.id = 77;
  msg.value = 99;

  MockProcessBlock block(TEST_NUM_SAMPLES);

  MIDIMap midiMap;
  midiMap.addEntry(msg, block);

  ProcessBlock * lookedupBlock = midiMap.lookup(msg);

  ASSERT_EQ(lookedupBlock,&block);
}

TEST(MIDIMap, AddEntry_and_Lookup_MultipleEntries)
{
  MIDI_Message_t msg1;
  msg1.type = 0x9;
  msg1.id = 77;
  msg1.value = 1;

  MIDI_Message_t msg2;
  msg2.type = 0x9;
  msg2.id = 88;
  msg2.value = 2;

  MockProcessBlock block1(TEST_NUM_SAMPLES);
  MockProcessBlock block2(TEST_NUM_SAMPLES);

  MIDIMap midiMap;
  midiMap.addEntry(msg1, block1);
  midiMap.addEntry(msg2, block2);

  ProcessBlock * lookedupBlock1 = midiMap.lookup(msg1);
  ProcessBlock * lookedupBlock2 = midiMap.lookup(msg2);

  ASSERT_EQ(lookedupBlock1,&block1);
  ASSERT_EQ(lookedupBlock2,&block2);
}
