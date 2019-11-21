#ifndef __FIRBLOCK_HPP__
#define __FIRBLOCK_HPP__

#include "ProcessBlock.hpp"

#include "stm32f769xx.h" //identifies device type for FPU_PRESENT
#include "arm_math.h"

class FIRProcessor {
  
};

class CircularFIRProcessor : public FIRProcessor{
  
};

class ARMDSPFIRProcessor : public FIRProcessor{
  
};



class FIRBlock : public ProcessBlock{
  float * filter_coefficients;
  float * firStateF32;
  arm_fir_instance_f32 S;
  FIRProcessor & firProcessor;

public:
  FIRBlock(const char * name, uint32_t size);
  FIRBlock(const char * name, uint32_t size, FIRProcessor & cfirp);

  void process(sample_t * samplesToProcess);
  void assignCoefficientArray(uint8_t midivalue);
  void setMIDIParameter(BlockParamIdentifier_t id, int value);
};


#endif
