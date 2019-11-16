#ifndef __ARMDSPFFTPROCESSOR_HPP__
#define __ARMDSPFFTPROCESSOR_HPP__

#include "FFTBlock.hpp"

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
    RUN_AND_LOG(arm_rfft_fast_init_f32(&S, size););
    RUN_AND_LOG(arm_rfft_fast_f32(&S, fft_buf, resultSpectrum, 0); ); //0 for fft, 1 for ifft
    RUN_AND_LOG(arm_cmplx_mag_f32(resultSpectrum, resultSpectrum, size/2);  );

    return fft_buf;
  }
};
#endif
