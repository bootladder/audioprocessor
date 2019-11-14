#include <iostream>
#include "gtest/gtest.h"

#include "BlockGraph.hpp"

TEST(BlockGraph, initialize)
{
  GainBlock gainBlock1("name", 1024);
  GainBlock gainBlock2("name", 1024);
  BlockGraph graph = {
    .start = &gainBlock1,
    .edges = {
      {&gainBlock1, &gainBlock2},
    },
    .end = &gainBlock2,
  };

  (void)graph;
}

TEST(BlockGraph_toEdgeListJSONString, EmptyGraphShowsEmptyJSONArray)
{
  BlockGraph graph = {};  //zero initialize empty graph.  pointless in app use

  char * str = graph.toEdgeListJSONString();

  const char * expected =
    "["
    "]\n";

  ASSERT_STREQ(str, expected);
}

TEST(BlockGraph_toEdgeListJSONString, OneEdgeGraph)
{
  GainBlock gainBlock1("block1", 1024);
  GainBlock gainBlock2("block2", 1024);

  BlockGraph graph = {
    .start = &gainBlock1,
    .edges = {
      {&gainBlock1, &gainBlock2},
      {0,0},
    },
    .end = &gainBlock2,
  };

  char * str = graph.toEdgeListJSONString();

  const char * expected =
    "["
    "{"
    "\"block\":\"block1\""
    "\"next\":\"block2\""
    "}"
    "]\n";

  ASSERT_STREQ(str, expected);
}

TEST(BlockGraph_toEdgeListJSONString, LotsOfEdgesGraph)
{
  GainBlock block1("block1", 1024);
  GainBlock block2("block2", 1024);
  GainBlock block3("block3", 1024);
  GainBlock block4("block4", 1024);

  BlockGraph graph = {
    .start = &block1,
    .edges = {
      {&block1, &block2},
      {&block1, &block3},
      {&block2, &block3},
      {&block3, &block4},
      {0,0},
    },
    .end = &block2,
  };

  char * str = graph.toEdgeListJSONString();

  const char * expected =
    "["
    "{"
    "\"block\":\"block1\""
    "\"next\":\"block2\""
    "}"
    ","
    "{"
    "\"block\":\"block1\""
    "\"next\":\"block3\""
    "}"
    ","
    "{"
    "\"block\":\"block2\""
    "\"next\":\"block3\""
    "}"
    ","
    "{"
    "\"block\":\"block3\""
    "\"next\":\"block4\""
    "}"
    "]\n";

  ASSERT_STREQ(str, expected);
}

