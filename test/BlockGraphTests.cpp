#include <iostream>
#include "gtest/gtest.h"

#include "BlockGraph.hpp"

TEST(BlockGraph, initialize)
{
  GainBlock gainBlock1(1024);
  GainBlock gainBlock2(1024);
  BlockGraph graph = {
    .start = &gainBlock1,
    .edges = {
      {&gainBlock1, &gainBlock2},
    },
    .end = &gainBlock2,
  };

}
