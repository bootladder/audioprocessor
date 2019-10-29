#include <iostream>
#include "gtest/gtest.h"

extern "C"{
#include "AudioProcessor.h"
}

TEST(sample_test_case, sample_test)
{
  static int16_t testBuf[3] = {1,2,3};
  static int16_t * out = AudioProcessor_ProcessSampleBuffer(testBuf,3);
  EXPECT_EQ(out[0], 1);
  EXPECT_EQ(out[1], 2);
  EXPECT_EQ(out[2], 3);
}
