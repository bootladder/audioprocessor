#ifndef __FIRCOEFFICIENTS_HPP__
#define __FIRCOEFFICIENTS_HPP__

#include <stdint.h>
#include "SamplingTypes.h"

class FIRCoefficients{
public:
  FIRCoefficients(sample_t * coeffValues, uint32_t numTaps)
    : num_taps(numTaps), coeffs(coeffValues) {}

  uint32_t num_taps;
  sample_t * coeffs;
};

#endif
