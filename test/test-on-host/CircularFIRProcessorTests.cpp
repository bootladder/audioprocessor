#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "FIRBlock.hpp"
#include "CircularFIRProcessor.hpp"

TEST(CircularFIRProcessor, inits_and_calls_calculate)
{
  DelayBuffer delayBuffer(1024);
  CircularFIRProcessor cfirp(delayBuffer);


  sample_t in[32];
  sample_t out[32];
  sample_t coeff_values[32];

  for(int i=0; i<32; i++){
    in[i] = 10;
    out[i] = 0;
    coeff_values[i] = 0.1;
    delayBuffer.insertSample(10);
  }

  FIRCoefficients coeffs(coeff_values, 32);
  cfirp.calculate(in, out, coeffs, 32);

  sample_t expectedOutZeroIndex = 10*32*0.1;
  ASSERT_EQ(expectedOutZeroIndex, out[0]);
  ASSERT_EQ(expectedOutZeroIndex, out[1]);
}
