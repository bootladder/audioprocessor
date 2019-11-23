#ifndef __ARMDSPFIRPROCESSOR_HPP__
#define __ARMDSPFIRPROCESSOR_HPP__

#include "stm32f769xx.h" //identifies device type for FPU_PRESENT
#include "arm_math.h"

//#include "constexpr_arm_sin_f32.hpp"

//firStateF32 = new float[MAX_BLOCK_SIZE + MAX_NUM_TAPS - 1];

//#include "LowPassFilterCoefficients.hpp"

//static constexpr struct LowPassFilterCoefficients low_pass_filter_coefficients = LowPassFilterCoefficients();


//S.pCoeffs = (float * )low_pass_filter_coefficients.arr[midivalue];

//static char str[100];
//int size = tfp_snprintf(str,100, "%s, Cutoff(Hz), %d\n", name, midi_value_to_cutoff(value));
//SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);



#include "FIRBlock.hpp"
class ARMDSPFIRProcessor : public FIRProcessor{

  // STATIC... DON'T DO THIS
  float firStateF32[3000];
  arm_fir_instance_f32 S;

  void calculate(const sample_t * const in, sample_t * out, const FIRCoefficients & coeffs, const uint32_t size)
  {
    arm_fir_init_f32(&S, coeffs.num_taps, (float*)coeffs.coeffs,
                     &firStateF32[0], size); //blocksize

    //the function modifies the input so have to make a copy
    sample_t *inputCopy = new sample_t[size];
    for(int i=0;i<size;i++)
      inputCopy[i] = in[i];

    arm_fir_f32(&S, inputCopy, out, size);
  }
};

#endif
