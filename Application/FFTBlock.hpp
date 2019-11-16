#ifndef __FFTBLOCK_HPP__
#define __FFTBLOCK_HPP__

#include "ProcessBlock.hpp"

extern "C"{
  #include "MemoryLogger.h"
}

class FFTProcessor {
public:
  virtual sample_t * calculateFFT(sample_t * buf, uint32_t size) = 0;
};


class FFTBlock : public RealProcessBlock {
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

    MemoryLogger_LogStringLn("FFT BLOCK INTIAILID");
  }

  void process(sample_t * samplesToProcess)
  {
    for(int i=(fft_buf_size - num_samples), j=0; i<fft_buf_size; i++){
      fft_buf[i] = samplesToProcess[j++];
    }
    LOG_ONESHOT("FFT ABOUT TO CALCULATE");
    fftProcessor.calculateFFT(fft_buf, fft_buf_size);
  }
};


#endif
