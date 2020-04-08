#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "gtest/gtest.h"
#include "MIDIReceiver.hpp"

#include <MIDI_Message.hpp>
#include <ProcessBlock.hpp>

class MockMIDIReceiver : public MIDIReceiver{
public:
    MockMIDIReceiver(): MIDIReceiver() {}
    MOCK_METHOD(void, setMIDIParameter, (BlockParamIdentifier_t id, int value));

};

TEST(MIDIReceiver, receives)
{
    MockMIDIReceiver midiReceiver;

    MIDI_Message_t msg1 = {1,2,3};
    midiReceiver.assignMIDIMessageToParameter(msg1, PARAM_0);

    MIDI_Message_t msg2 = {2,3,4};
    midiReceiver.assignMIDIMessageToParameter(msg2, PARAM_1);

    EXPECT_CALL(midiReceiver, setMIDIParameter(PARAM_0,msg1.value)).Times(1);
    EXPECT_CALL(midiReceiver, setMIDIParameter(PARAM_1,msg2.value)).Times(1);

    midiReceiver.MIDIMessageReceived(msg1);
    midiReceiver.MIDIMessageReceived(msg2);
}