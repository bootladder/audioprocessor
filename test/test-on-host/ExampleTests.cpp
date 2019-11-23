#include <iostream>
#include "gtest/gtest.h"

extern "C"{
}

TEST(sample_test_case, sample_test)
{
  EXPECT_EQ(1, 1);
}
