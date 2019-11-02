#include "ProcessBlockFunctions_FIRFilters.hpp"
#include "stm32f769xx.h" //identifies device type for FPU_PRESENT
#include "arm_math.h"

static arm_fir_instance_q15 S;
#define SAMPLE_FREQUENCY 48000.0
#define MAX_BLOCK_SIZE 1024
#define MAX_NUM_TAPS 1024

void ProcessBlockFunctions_FIRLowPass_CalculateCoefficients(uint32_t cutoff)
{
  static int16_t coeffs[MAX_NUM_TAPS];
  int16_t fc_normalized = (int16_t)cutoff / SAMPLE_FREQUENCY;
  int16_t coefficient_sum = 0.0;
  int numTaps = 300;

  for(int i=0; i<numTaps;i++)
    {
      int n = i - (numTaps/2);  //apply a time shift of numTaps/2
      if(n==0){  //sinc(1) = 1
        coeffs[i] = 1;
        coefficient_sum += coeffs[i];
        continue;
      }
      int16_t x = 3.14*fc_normalized*(int16_t)n;
      coeffs[i] = fc_normalized * arm_sin_q15(x) / x;

      coefficient_sum += coeffs[i];
    }

  //normalize coefficients to 1
  for(int i=0;i<numTaps;i++){
    coeffs[i] = coeffs[i] / coefficient_sum;
  }
}

void ProcessBlockFunctions_FIRLowPass(BlockState * state, int16_t * in, int16_t * out, uint32_t size)
{
  arm_fir_q15(&S, in, out, size);
}

