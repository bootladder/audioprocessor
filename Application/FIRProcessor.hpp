#ifndef __FIRPROCESSOR_HPP__
#define __FIRPROCESSOR_HPP__

#include "SamplingTypes.h"
#include "FIRCoefficients.hpp"
#include <stdint.h>

class FIRProcessor {
public:
  virtual void calculate(const sample_t * const in, sample_t * out, const FIRCoefficients & coeffs, const uint32_t size) = 0;
};


#endif
