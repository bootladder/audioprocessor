#ifndef __FIRBLOCK_HPP__
#define __FIRBLOCK_HPP__


#include "ProcessBlockFunctions_FIRFilters.hpp"
#include "BSP_Audio_Buffer_Interface.h"
#include "stm32f769xx.h" //identifies device type for FPU_PRESENT
#include "arm_math.h"

#include "ProcessBlock.hpp"

extern "C"{
#include "SerialLogger.h"
#include "MemoryLogger.h"
}

#define SAMPLE_FREQUENCY 48000.0
#define MAX_BLOCK_SIZE 2048
#define MAX_NUM_TAPS 2048


class FIRBlock : public RealProcessBlock{
  sample_t * filter_coefficients;
  arm_fir_instance_f32 S;

public:
  FIRBlock(uint32_t size) : RealProcessBlock(ProcessBlockFunctions_Gain2X, size){
    MemoryLogger_LogStringLn("FIR Coeffs calcualtd");
    calculateCoefficients(200);
  }

  void process(sample_t * samplesToProcess){
    LOG_ONESHOT("FIR PRocess");
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }
    arm_fir_f32(&S, inputBuffer, outputBuffer, num_samples);
  }

  void calculateCoefficients(uint32_t cutoff)
  {
    static float coeffs[MAX_NUM_TAPS];
    float fc_normalized = (float) (cutoff / SAMPLE_FREQUENCY);
    float coefficient_sum = 0.0;
    int numTaps = 1024;

    for(int i=0; i<numTaps;i++)
      {
        int n = i - (numTaps/2);  //apply a time shift of numTaps/2
        if(n==0){  //sinc(1) = 1
          coeffs[i] = 1;
          coefficient_sum += coeffs[i];
          continue;
        }
        float x = 3.1415927*fc_normalized*(float)n;
        coeffs[i] = fc_normalized * arm_sin_f32(x) / x;

        coefficient_sum += coeffs[i];
      }

    //normalize coefficients to 1
    for(int i=0;i<numTaps;i++){
      coeffs[i] = coeffs[i] / coefficient_sum;
    }

    static float firStateF32[MAX_BLOCK_SIZE + MAX_NUM_TAPS - 1];
    arm_fir_init_f32(&S, numTaps, coeffs, &firStateF32[0], MY_PROCESSING_BUFFER_SIZE_SAMPLES); //blocksize
  }

};
#endif
