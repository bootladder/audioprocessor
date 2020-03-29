#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "LFO.hpp"
#include "MIDI_Message.hpp"

MIDI_Message_t mockMIDIMessage;
void mockMIDIMessageHandlerFunc(MIDI_Message_t message){
    mockMIDIMessage = message;
}

bool mockStartTimerMsFunc_called;
void mockStartTimerMsFunc(LFO * lfo, int ms){
    mockStartTimerMsFunc_called = true;
}

TEST(LFO, initialize)
{
    LFO lfo("name");
    lfo.setLFOFrequencyHz(1000);
    lfo.setMIDIMessage({MIDI_CONTROL_CHANGE,20,1});
    lfo.setMIDIMessageHandlerFunc(mockMIDIMessageHandlerFunc);

    lfo.setStartTimerMsFunc(mockStartTimerMsFunc);
    lfo.startTimerMs(1);


    lfo.tickCallback();


    MIDI_Message_t expectedMIDIMessage = {MIDI_CONTROL_CHANGE, 20, 2};
    ASSERT_EQ(mockMIDIMessage.id, expectedMIDIMessage.id);
    ASSERT_EQ(mockMIDIMessage.type, expectedMIDIMessage.type);
}


// for a triangle wave, half a period is max amplitude
// using the absolute value of sine wave,
// it starts high, zero in the middle
TEST(LFO, waveform)
{
    LFO lfo("name");
    lfo.setLFOFrequencyHz(10);
    lfo.setMIDIMessage({MIDI_CONTROL_CHANGE,20,1});
    lfo.setMIDIMessageHandlerFunc(mockMIDIMessageHandlerFunc);

    lfo.setStartTimerMsFunc(mockStartTimerMsFunc);
    lfo.startTimerMs(10);

    //5 times is half a period to 10hz @ 10ms
    //down to zero
    for(int i=0; i<5; i++){
        lfo.tickCallback();
    }

    int expectedMIDIValue = 0;
    ASSERT_EQ(mockMIDIMessage.value, expectedMIDIValue);

    //5 MORE times is a FULL period
    // back up to 127
    for(int i=0; i<5; i++){
        lfo.tickCallback();
    }

    expectedMIDIValue = 127;
    ASSERT_EQ(mockMIDIMessage.value, expectedMIDIValue);

    //5 MORE times is back to zero
    for(int i=0; i<5; i++){
        lfo.tickCallback();
    }

    expectedMIDIValue = 0;
    ASSERT_EQ(mockMIDIMessage.value, expectedMIDIValue);
}