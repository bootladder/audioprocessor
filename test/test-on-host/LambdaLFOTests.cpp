#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "LambdaLFO.hpp"

#include "IIRBlock.hpp"

static bool mockStartTimerMsFunc_called;
static void mockStartTimerMsFunc(LFO & lfo, int ms){
    (void)lfo; (void)ms;
    mockStartTimerMsFunc_called = true;
}


TEST(LambdaLFO, callsLambda)
{
    LambdaLFO lfo("name");
    lfo.setLFOFrequencyHz(1000);
    lfo.setStartTimerMsFunc(mockStartTimerMsFunc);
    lfo.startTimerMs(1);


    IIRBlock iir1("iir1", 1024);
    auto lambda = [block = &iir1](int f){block->setCutoffFrequency(f);};

    lfo.setLambda(lambda);

    lfo.tickCallback();

    ASSERT_EQ(iir1.getCutoffFrequency(), lfo.getCurrentLFOValue());
}



TEST(LambdaLFO, canSetBounds)
{
    LambdaLFO lfo("name");
    lfo.setLFOFrequencyHz(10);
    lfo.setStartTimerMsFunc(mockStartTimerMsFunc);
    lfo.startTimerMs(1);
    lfo.setMidPoint(500);
    lfo.setAmplitude(200);


    IIRBlock iir1("iir1", 1024);
    auto lambda = [block = &iir1](int f){block->setCutoffFrequency(f);};

    lfo.setLambda(lambda);

    lfo.tickCallback();

    ASSERT_LE(lfo.getCurrentLFOValue(), 700);
    ASSERT_GE(lfo.getCurrentLFOValue(), 400);

    for(int i=0; i<1000;i++){
      lfo.tickCallback();
      //printf("lfo value[%d] = %d\n", i, lfo.getCurrentLFOValue());
    }
}


