#include "ProcessBlock.hpp"

#define MAX_BLOCKGRAPH_EDGES 32

struct BlockGraphEdge{
  ProcessBlock * block;
  ProcessBlock * next;
};

struct BlockGraph{
  ProcessBlock * start;
  BlockGraphEdge edges[MAX_BLOCKGRAPH_EDGES];
  ProcessBlock * end;
};
