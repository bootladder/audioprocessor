#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "LambdaLFO.hpp"

#include "IIRBlock.hpp"

#define LOGGING_OFF

#include <stdarg.h>
static void logf(const char *format, ...)
{
  va_list args;
  va_start(args, format);

#ifdef LOGGING_ON
  vprintf(format, args);
#endif
  va_end(args);
}


TEST(LambdaLFO, callsLambda)
{
    LambdaLFO lfo("name");
    lfo.setLFOFrequencyHz(1000);
    lfo.setTickPeriodMillis(1);


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
    lfo.setTickPeriodMillis(1);
    lfo.setMidPoint(500);
    lfo.setAmplitude(200);


    IIRBlock iir1("iir1", 1024);
    auto lambda = [block = &iir1](int f){block->setCutoffFrequency(f);};

    lfo.setLambda(lambda);

    lfo.tickCallback();

    for(int i=0; i<1000;i++){
      lfo.tickCallback();
      ASSERT_LE(lfo.getCurrentLFOValue(), 700);
      ASSERT_GE(lfo.getCurrentLFOValue(), 400);
      logf("lfo value[%d] = %d\n", i, lfo.getCurrentLFOValue());
    }
}
