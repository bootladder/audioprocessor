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
    resultSpectrum[37] = 20480; // fake max
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

MATCHER_P2(DoesArrayContainBufAtIndex, buf, index, "") {
  for(int i=0; i<NUM_SAMPLES/8; i++){
    //cout << "arg is " << arg[index+i] << "and buf is " << buf[i] << endl;
    if(arg[index + i] != buf[i])
      return false;
  }
  return true;
}

TEST(FFTBlock, applies_fft_on_larger_buffer)
{
  int fft_buf_size = 1024*4;
  static MockFFTProcessor mockFFTProcessor;
  FFTBlock block = FFTBlock("name",mockFFTProcessor, fft_buf_size, NUM_SAMPLES);

  //input a staircase into the FFT block.
  for(int i=0;i<NUM_SAMPLES;i++){
    testBuf[i] = i;
  }

  //input should be downsampled by 8 before running the FFT
  static sample_t downSampledTestBuf[NUM_SAMPLES/8];
  for(int i=0;i<NUM_SAMPLES/8;i++){
    downSampledTestBuf[i] = i * 8;
  }


  //buffer argument starts with mostly zeros and ends with the sample buffer
  EXPECT_CALL(mockFFTProcessor, calculateFFT(AllOf(IsValidPointer(),
                                                   IsArrayMostlyZeros(),
                                                   DoesArrayContainBufAtIndex(downSampledTestBuf, 1024*4 - NUM_SAMPLES/8)
                                                   ),
                                             _,  //don't car about spectrum param
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
  int expectedPeakFreq = 37.0*48000.0/8.0/fft_buf_size;
  ASSERT_EQ(peakFreq, expectedPeakFreq);

  int peakMag = block.getSpectrumPeakMagnitude();
  int expectedMag = 20480 * 8 / NUM_SAMPLES;

  ASSERT_EQ(peakMag, expectedMag);
}
