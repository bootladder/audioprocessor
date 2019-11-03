#include <iostream>
using namespace std;
#include "gtest/gtest.h"

#include "MIDIMap.hpp"
#include "ProcessBlock.hpp"
#include "ProcessBlockFunctions.hpp"
#define TEST_NUM_SAMPLES 1024

TEST(MIDIMap, AddEntry_and_Lookup)
{
  MIDI_Message_t msg;
  msg.type = 0x9;
  msg.id = 77;
  msg.value = 99;

  RealProcessBlock block(ProcessBlockFunctions_GainParameterized, TEST_NUM_SAMPLES);

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

  RealProcessBlock block1(ProcessBlockFunctions_GainParameterized, TEST_NUM_SAMPLES);
  RealProcessBlock block2(ProcessBlockFunctions_GainParameterized, TEST_NUM_SAMPLES);

  MIDIMap midiMap;
  midiMap.addEntry(msg1, block1);
  midiMap.addEntry(msg2, block2);

  ProcessBlock * lookedupBlock1 = midiMap.lookup(msg1);
  ProcessBlock * lookedupBlock2 = midiMap.lookup(msg2);

  ASSERT_EQ(lookedupBlock1,&block1);
  ASSERT_EQ(lookedupBlock2,&block2);
}
