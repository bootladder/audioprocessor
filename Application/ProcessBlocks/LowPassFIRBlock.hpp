#ifndef __LOWPASSFIRBLOCK_HPP__
#define __LOWPASSFIRBLOCK_HPP__

#include "ProcessBlock.hpp"
#include "LowPassFilterCoefficients.hpp"

class LowPassFIRBlock : public FIRBlock{
public:
  LowPassFIRBlock(const char * name, uint32_t size, FIRProcessor & firp):
    FIRBlock(name,size, firp){
  }

  //static constexpr
  //struct LowPassFilterCoefficients
  //low_pass_filter_coefficients = LowPassFilterCoefficients();

  void setFrequency(uint32_t freq){
    (void)freq;
    //updateCoefficients(freq);
  }
};

#endif
