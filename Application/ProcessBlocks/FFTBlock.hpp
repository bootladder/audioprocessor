#ifndef __FFTBLOCK_HPP__
#define __FFTBLOCK_HPP__

#include "ProcessBlock.hpp"

extern "C"{
#include "MemoryLogger.h"
#include "SerialLogger.h"
#include "tinyprintf.h"
}

class FFTProcessor {
public:
  virtual sample_t * calculateFFT(sample_t * fft_buf, sample_t * resultSpectrum, uint32_t size) = 0;
};


class FFTBlock : public ProcessBlock
{
private:
  FFTProcessor & fftProcessor;
  int fft_buf_size;
  sample_t * fft_buf;
  sample_t * spectrum;

  int binRange;
  int max;
  int max_index;

public:
  FFTBlock(const char * name, FFTProcessor & f, int fftBufSize, uint32_t size) :
    ProcessBlock(name, size),
    fftProcessor(f),
    fft_buf_size(fftBufSize)
  {
    fft_buf = new sample_t[fftBufSize];
    spectrum = new sample_t[fftBufSize];
    binRange = fftBufSize;
    for(auto i=0; i<fftBufSize; i++)
      fft_buf[i] = 0;
  }

  void process(sample_t * samplesToProcess)
  {
    //downsample the sample buffer by 8        HARD CODED 8!!!!
    for(uint32_t i=0; i<num_samples/8; i++){
      samplesToProcess[i] = samplesToProcess[i*8];
    }


    //zero pad the lower part of the FFT buffer
    for(uint32_t i=0; i < (fft_buf_size - (num_samples/8)) ; i++){
      fft_buf[i] = 0;
    }

    // load the upper part of the FFT buffer with the sample buffer
    for(int i=(fft_buf_size - (num_samples/8)), j=0; i<fft_buf_size; i++){
      fft_buf[i] = samplesToProcess[j++];
    }

    fftProcessor.calculateFFT(fft_buf, spectrum, fft_buf_size);

    //find the peak (not necessarily the fundamental)
    //don't look higher than range
    max = 0;
    for(int i=0; i<binRange; i++){
      if( my_abs(spectrum[i]) > max){ //need portable absd
        max = (int)spectrum[i];
        max_index = i;
      }
    }
    max = max / fft_buf_size; //scale it down
    //multiply by duty cycle, ie. ratio of zero pad
    float duty_num_samples = num_samples/8;
    float factor = fft_buf_size / duty_num_samples;
    max = max * factor;

    static char str[100];
    int size = tfp_snprintf(str,100, "Mag:  %08d,  Index:  %04d, Freq: %04d\n", max, max_index, getSpectrumPeakFreq());
    SerialLogger_Log(LOGTYPE_EVENT, (uint8_t *)str, size);
  }

  sample_t * getSpectrum(){return spectrum;}

  int getSpectrumPeakFreq(){
    //freq bins are sample rate / fft size
    float freqfloat = max_index *(48000.0/8) /2048.0;
    int freq = (int)freqfloat;
    return freq;
  }
  int getSpectrumPeakMagnitude(){
    return max;
  }

  //lol portability
  sample_t my_abs(sample_t in){
    if(in < 0){
      return in*-1.0;
    }
    return in;
  }
};


#endif
