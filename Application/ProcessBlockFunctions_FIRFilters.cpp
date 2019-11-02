#include "ProcessBlockFunctions_FIRFilters.hpp"
#include "BSP_Audio_Buffer_Interface.h"
#include "stm32f769xx.h" //identifies device type for FPU_PRESENT
#include "arm_math.h"

static arm_fir_instance_f32 S;
#define SAMPLE_FREQUENCY 48000.0
#define MAX_BLOCK_SIZE 2048
#define MAX_NUM_TAPS 2048

void ProcessBlockFunctions_FIRLowPass_CalculateCoefficients(uint32_t cutoff)
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

void ProcessBlockFunctions_FIRLowPass(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  arm_fir_f32(&S, in, out, size);
}

