#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "FIRBlock.hpp"
#include "CoefficientTable.hpp"

class MockFIRProcessor : public FIRProcessor {
public:
  MOCK_METHOD(void, calculate, (const sample_t * const in, sample_t * out, const FIRCoefficients & coeffs, const uint32_t size), (override));
};


class MockCoefficientTable : public CoefficientTable {
public:

  MOCK_METHOD(sample_t *, lookupCutoffFrequency, (int freq) , (override));
};


#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];


MATCHER_P(FIRCoefficients_PointerMatches, ptr, "") {
  return arg.coeffs == ptr;
}

TEST(FIRBlock, inits_and_calls_calculate)
{
  MockFIRProcessor mockFIRProcessor;
  MockCoefficientTable mockCoefficientTable;
  FIRBlock block("name", NUM_SAMPLES, mockFIRProcessor, mockCoefficientTable);

  sample_t * coefficientsPointer = (sample_t *) 3333;

  EXPECT_CALL(mockCoefficientTable, lookupCutoffFrequency(1000)).WillOnce(Return(coefficientsPointer));
  EXPECT_CALL(mockFIRProcessor, calculate(_, _,

                                          FIRCoefficients_PointerMatches(coefficientsPointer),
                                           _));

  block.setCutoffFrequency(1000);
  block.process(testBuf);
}

