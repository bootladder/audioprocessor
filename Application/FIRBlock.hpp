#ifndef __FIRBLOCK_HPP__
#define __FIRBLOCK_HPP__

#include "BSP_Audio_Buffer_Interface.h"
#include "stm32f769xx.h" //identifies device type for FPU_PRESENT
#include "arm_math.h"

#include "constexpr_arm_sin_f32.hpp"

#include "ProcessBlock.hpp"

extern "C"{
#include "MemoryLogger.h"
}

#define SAMPLE_FREQUENCY 48000.0
#define MAX_BLOCK_SIZE 2048
#define MAX_NUM_TAPS 512



class FIRBlock : public RealProcessBlock{
  float * filter_coefficients;
  float * firStateF32;
  arm_fir_instance_f32 S;

public:
  FIRBlock(const char * name, uint32_t size);

  void process(sample_t * samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }
    arm_fir_f32(&S, inputBuffer, outputBuffer, num_samples);
  }

  void assignCoefficientArray(uint8_t midivalue);
  void setMIDIParameter(BlockParamIdentifier_t id, int value);

};


#endif
