#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "ProcessBlock.hpp"

class FFTProcessor {
public:
  virtual sample_t * calculateFFT(sample_t * buf, uint32_t size) = 0;
};


class FFTBlock : RealProcessBlock {
private:
  FFTProcessor & fftProcessor;
  int fft_buf_size;
  sample_t * fft_buf;
public:
  FFTBlock(const char * name, FFTProcessor & f, int fftBufSize, uint32_t size) :
    RealProcessBlock(name, size),
    fftProcessor(f),
    fft_buf_size(fftBufSize)
  {
    fft_buf = new sample_t[fftBufSize];
    for(auto i=0; i<fftBufSize; i++)
      fft_buf[i] = 0;
  }

  void process(sample_t * samplesToProcess)
  {
    for(int i=(fft_buf_size - num_samples), j=0; i<fft_buf_size; i++){
      fft_buf[i] = samplesToProcess[j++];
    }
    fftProcessor.calculateFFT(fft_buf, fft_buf_size);
  }
};


class MockFFTProcessor : public FFTProcessor{
public:
  MockFFTProcessor(){;}
  MOCK_METHOD(sample_t *, calculateFFT, (sample_t * buf, uint32_t size), (override));
};

#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];


TEST(FFTBlock, inits_and_calls_calculate)
{
  static MockFFTProcessor mockFFTProcessor;
  FFTBlock block = FFTBlock("name",mockFFTProcessor, 10000, NUM_SAMPLES); //large number

  EXPECT_CALL(mockFFTProcessor, calculateFFT(_, _));

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
                                             fft_buf_size));

  block.process(testBuf);
}
