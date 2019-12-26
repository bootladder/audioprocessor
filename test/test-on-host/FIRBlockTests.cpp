#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "FIRBlock.hpp"
#include "CoefficientTable.hpp"
#include "LowPassCoefficientTable.hpp"

class MockFIRProcessor : public FIRProcessor {
public:
  MOCK_METHOD(void, calculate, (const sample_t * const in, sample_t * out, const FIRCoefficients & coeffs, const uint32_t size), (override));
};


class MockCoefficientTable : public CoefficientTable {
public:

  MOCK_METHOD(sample_t *, lookupCutoffFrequency, (int freq) , (override));
  MOCK_METHOD(int, getNumTaps, () , (override));
};


#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];


MATCHER_P2(FIRCoefficients_PointerMatches_And_NumTapsMatches, ptr, numTaps, "") {
  return arg.coeffs == ptr && arg.num_taps == numTaps;
}

TEST(FIRBlock, inits_and_calls_calculate)
{
  MockFIRProcessor mockFIRProcessor;
  MockCoefficientTable mockCoefficientTable;
  FIRBlock block("name", NUM_SAMPLES, mockFIRProcessor, mockCoefficientTable);

  sample_t * coefficientsPointer = (sample_t *) 3333;
  uint32_t expectedNumTaps = 100;

  EXPECT_CALL(mockCoefficientTable, lookupCutoffFrequency(1000)).WillOnce(Return(coefficientsPointer));
  EXPECT_CALL(mockCoefficientTable, getNumTaps()).WillOnce(Return(100));
  block.setCutoffFrequency(1000);


  EXPECT_CALL(mockFIRProcessor, calculate(_, _,

            FIRCoefficients_PointerMatches_And_NumTapsMatches(coefficientsPointer, expectedNumTaps),
                                          _));

  block.process(testBuf);
}

TEST(LowPassCoefficientTable, constructs)
{
  static LowPassCoefficientTable lowPassCoefficientTable;
}

