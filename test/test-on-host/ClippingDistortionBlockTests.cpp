#include <iostream>
using namespace std;
#include "gtest/gtest.h"

#include "ClippingDistortionBlock.hpp"


#define not_LOGGING_ON
#include <stdarg.h>
static int logf(const char *format, ...)
{
  va_list args;
  va_start(args, format);

#ifdef LOGGING_ON
  vprintf(format, args);
#endif
  va_end(args);
}

#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];

static void fillTestBufferWithRampFunction(sample_t * buf){
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    buf[i] = (sample_t)(-0x8000 + ((float)i*0x10000)/NUM_SAMPLES);
  }
}


TEST(ClippingDistortionBlock, clippingdistortion_setClipFactor_outputIsClipped)
{
  ClippingDistortionBlock block("name", NUM_SAMPLES, ClippingDistortionBlock::CLIPPING_TYPE_HARD);

  sample_t clippingFactor = 0.8;
  block.setClippingFactor(clippingFactor);
  sample_t max = 0x8000 * clippingFactor;
  sample_t min = -1*0x8000 * clippingFactor;

  fillTestBufferWithRampFunction(testBuf);

  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  //ASSERT_EQ(0x4000, 0x8000 * clippingFactor);

  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_LE(out[i], max);
    ASSERT_GE(out[i], min);
  }
}


TEST(ClippingDistortionBlock, SoftClipping_setClipFactor_OutputIsClipped)
{
  ClippingDistortionBlock block("name", NUM_SAMPLES, ClippingDistortionBlock::CLIPPING_TYPE_SOFT);

  sample_t clippingFactor = 0.8;
  block.setClippingFactor(clippingFactor);
  sample_t max = 0x8000 * clippingFactor;
  sample_t min = -1*0x8000 * clippingFactor;

  fillTestBufferWithRampFunction(testBuf);

  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  //ASSERT_EQ(0x4000, 0x8000 * clippingFactor);
  for(uint32_t i=0; i<NUM_SAMPLES; i++){

    logf("testBuf[%d] = %f\n", i, testBuf[i]);
    logf("out[%d] = %f\n", i, out[i]);
  }

  //no values outside the bounds
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    ASSERT_LE(out[i], max);
    ASSERT_GE(out[i], min);

    //printf("out[%d] = %f\n", i, out[i]);
  }

  //but they can't all be zero for a ramp input.
  //there shld be values greater than half the bounds
  bool foundValueGreaterThanHalfBounds = false;
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    if(out[i] > max/2)
      foundValueGreaterThanHalfBounds = true;
  }

  ASSERT_TRUE(foundValueGreaterThanHalfBounds);

}

TEST(ClippingDistortionBlock, SoftClipping_WithinThreshold_AppliesWaveShapingFunction)
{
//  ClippingDistortionBlock block("name", NUM_SAMPLES, ClippingDistortionBlock::CLIPPING_TYPE_SOFT);
//
//  fillTestBufferWithRampFunction(testBuf);
//
//  block.process(testBuf);
//  sample_t * out = block.getOutputBuffer();

  // shaping function
  //sample_t testSample = testBuf[500]/32000.0; //normalize to 1
  //sample_t expectedOutput = (testSample - (testSample*testSample*testSample / 3)) * 32000.0;
  //ASSERT_FLOAT_EQ(out[500], expectedOutput);

  // clip .. how to clip?
  //sample_t sampleExpectedToBeClipped = out[1000];
  //sample_t expectedClippedValue = (sample_t)0x8000  * (sample_t) 2 / (sample_t) 3;
  //ASSERT_FLOAT_EQ(sampleExpectedToBeClipped, expectedClippedValue);
}



TEST(ClippingDistortionBlock, FloatingPointSanityCheck)
{
  float sample1 = (float)3.14;
  logf("sample1 = %f\n", sample1);
  logf("sample1 = %a\n", sample1);
  uint32_t i = 100;
  logf("wtf = %f\n", (sample_t)(-0x8000 + ((float)i*0x10000)/NUM_SAMPLES));
  sample_t expectedClippedValue = (sample_t)0x8000 * (sample_t)2 / (sample_t) 3;
  logf("expectedClippedValue = %f\n", expectedClippedValue);
 
}
