#ifndef __ARMDSPFFTPROCESSOR_HPP__
#define __ARMDSPFFTPROCESSOR_HPP__

#include "FFTBlock.hpp"

extern "C"{
#include "stm32f769xx.h"
#include "arm_math.h"
#include "MemoryLogger.h"
}

#define MAXIMUM_FFT_SIZE 10*1024
static arm_rfft_fast_instance_f32 S;

static float originalBufCopy[MAXIMUM_FFT_SIZE];
static float fftResult[MAXIMUM_FFT_SIZE];
static float fftMagnitudes[MAXIMUM_FFT_SIZE/2];  //convert complex result to magnitude


class ARMDSPFFTProcessor : public FFTProcessor {
private:
  //find the peak (fundamental?)
  //don't look higher than range
  int binRange = 256;
  int max = 0;
  int max_index = 0;

public:
  sample_t * calculateFFT(sample_t * buf, uint32_t size)
  {
    (void)buf;(void)size;

    //Copy the buffer because FFT modifies it
    for(uint32_t i=0; i<size; i++){
      originalBufCopy[i] = buf[i];
    }
    LOG_ONESHOT("ORIGI BUF COPY OK");

    RUN_AND_LOG(arm_rfft_fast_init_f32(&S, size););
    RUN_AND_LOG(arm_rfft_fast_f32(&S, originalBufCopy, fftResult, 0); ); //0 for fft, 1 for ifft
    RUN_AND_LOG(arm_cmplx_mag_f32(fftResult, fftMagnitudes, size/2);  );


    max = 0;
    for(int i=0; i<binRange; i++){
      if( abs(fftMagnitudes[i]) > max){
        max = (int)fftMagnitudes[i];
        max_index = i;
      }
    }
    max = max / size; //scale it down

    return buf;
  }
};
#endif
