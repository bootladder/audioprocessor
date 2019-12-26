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


#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];

TEST(FIRBlock, inits_and_calls_calculate)
{
  MockFIRProcessor mockFIRProcessor;
  FakeCoefficientTable fakeCoefficientTable;
  FIRBlock block("name", NUM_SAMPLES, mockFIRProcessor, fakeCoefficientTable);

  EXPECT_CALL(mockFIRProcessor, calculate(_, _, _, _));

  block.process(testBuf);
}

