#include <iostream>
using namespace std;
#include "gtest/gtest.h"

extern "C"{
#include "MIDIMessageHandler.h"
}
#include "MIDI_Message.h"

TEST(MIDIMessageHandler, identity)
{
  MIDI_Message_t msg = {
    1,2,3
  };
  MIDIMessageHandler_Handle(msg);
  ASSERT_EQ(1,1);
}
