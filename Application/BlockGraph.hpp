#include "ProcessBlock.hpp"

extern "C"{
  #include "tinyprintf.h"
}

#define MAX_BLOCKGRAPH_EDGES 32

struct BlockGraphEdge{
  ProcessBlock * block;
  ProcessBlock * next;
};

struct BlockGraph{
public:
  ProcessBlock * start;
  BlockGraphEdge edges[MAX_BLOCKGRAPH_EDGES];
  ProcessBlock * end;

  char * toEdgeListJSONString(void);
};




char * BlockGraph::toEdgeListJSONString(void)
{
  char * json_array_format =
    (char *)
    "["
    "%s"
    "]\n";

  static const char * edge_format =
    "{"
    "\"block\":\"%s\""
    "\"next\":\"%s\""
    "}"
    "%s";  //for the stupid comma

  static char edgelist[1024];
  int edgelist_index = 0;
  int i=0;
  while(edges[i].block != 0){

    const char * comma_or_not = (edges[i+1].block != 0) ? "," : "";

    int size = tfp_snprintf(&edgelist[edgelist_index], 1024, edge_format,
                            edges[i].block -> getName(),
                            edges[i].next -> getName(),
                            comma_or_not
                            );
    edgelist_index += size;
    i++;
  }

  static char output[1024];
  tfp_snprintf(output, 1024, json_array_format, edgelist);
  return output;
}
