#ifndef __ARMDSPFFTPROCESSOR_HPP__
#define __ARMDSPFFTPROCESSOR_HPP__

#include "FFTBlock.hpp"
#include <memory>

extern "C"{
#include "stm32f769xx.h"
#include "arm_math.h"
#include "MemoryLogger.h"
}

class ARMDSPFFTProcessor : public FFTProcessor {

private:
  arm_rfft_fast_instance_f32 S;

public:
  sample_t * calculateFFT(sample_t * fft_buf, sample_t * resultSpectrum, uint32_t size)
  {
    //this is a 32bit float operation so do a conversion
    //if sample_t is a double.
    auto f32inputBuf = std::make_unique<float32_t[]>(size);
    for(uint32_t i=0; i<size; i++){
      f32inputBuf[i] = (float32_t)fft_buf[i];
    }

    arm_rfft_fast_init_f32(&S, size);
    arm_rfft_fast_f32(&S, f32inputBuf.get(), resultSpectrum, 0);  //0 for fft, 1 for ifft
    arm_cmplx_mag_f32(resultSpectrum, resultSpectrum, size/2);

    return fft_buf;
  }
};
#endif
