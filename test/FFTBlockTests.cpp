#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "FFTBlock.hpp"


class MockFFTProcessor : public FFTProcessor{
public:
  MockFFTProcessor(){;}
  MOCK_METHOD(sample_t *, calculateFFT, (sample_t * fft_buf, sample_t * resultSpectrum, uint32_t size), (override));
};

class FakeFFTProcessor : public FFTProcessor{
public:
  FakeFFTProcessor(){;}

  sample_t * calculateFFT(sample_t * buf, sample_t * resultSpectrum, uint32_t size){
    //just attempt to write to all the buf locations,
    //to check for segfaults
    for(uint32_t i=0; i<size; i++){
      buf[i] = 3;
      resultSpectrum[i] = 99;
    }
    resultSpectrum[37] = 200;
    return buf;
  }

};

#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];


TEST(FFTBlock, inits_and_calls_calculate)
{
  static MockFFTProcessor mockFFTProcessor;
  FFTBlock block = FFTBlock("name",mockFFTProcessor, 10000, NUM_SAMPLES); //large number

  EXPECT_CALL(mockFFTProcessor, calculateFFT(_, _, _));

  block.process(testBuf);
}




// Sample buffers going into the block may be of size 1024
// but the FFT needs to operate on larger buffers, say 10k
// in order to get the frequency resolution.
// but, to keep time latency reasonable, we will
// zero pad all of the 10k buffer.

MATCHER(IsValidPointer, "") {
  return arg != 0;
}

MATCHER(IsArrayMostlyZeros, "") {
  for(int i=0; i<1024; i++){
    if(arg[i] != 0)
      return false;
  }
  return true;
}

MATCHER(DoesArrayContainSampleBuf, "") {
  for(int i=0; i<4096; i++){
    if(arg[i] == 6)
      return true;
  }
  return false;
}

TEST(FFTBlock, applies_fft_on_larger_buffer)
{
  int fft_buf_size = 1024*4;
  static MockFFTProcessor mockFFTProcessor;
  FFTBlock block = FFTBlock("name",mockFFTProcessor, fft_buf_size, NUM_SAMPLES);

  for(int i=0;i<NUM_SAMPLES;i++){
    testBuf[i] = 6;
  }

  //buffer argument starts with mostly zeros and ends with the sample buffer
  EXPECT_CALL(mockFFTProcessor, calculateFFT(AllOf(IsValidPointer(), IsArrayMostlyZeros(), DoesArrayContainSampleBuf()),
                                             _,
                                             fft_buf_size));

  block.process(testBuf);
}

TEST(FFTBlock, fake_fft_does_not_segfault_on_larger_buffer)
{
  int fft_buf_size = 1024*16;
  static FakeFFTProcessor fakeFFTProcessor;
  FFTBlock block = FFTBlock("name",fakeFFTProcessor, fft_buf_size, NUM_SAMPLES);

  block.process(testBuf);
}

TEST(FFTBlock, getSpectrum)
{
  int fft_buf_size = 2048;;
  static FakeFFTProcessor fakeFFTProcessor;
  FFTBlock block = FFTBlock("name", fakeFFTProcessor, fft_buf_size, NUM_SAMPLES);
  block.process(testBuf);

  sample_t * spectrum = block.getSpectrum();

  ASSERT_EQ(spectrum[0], 99);
}

TEST(FFTBlock, getSpectrumPeakFreq_and_getSpectrumPeakMagnitude)
{
  int fft_buf_size = 2048;;
  static FakeFFTProcessor fakeFFTProcessor;
  FFTBlock block = FFTBlock("name", fakeFFTProcessor, fft_buf_size, NUM_SAMPLES);
  block.process(testBuf);

  int peakFreq = block.getSpectrumPeakFreq();
  int peakMag = block.getSpectrumPeakMagnitude();

  ASSERT_EQ(peakFreq, 37);
  ASSERT_EQ(peakMag, 200);
}
