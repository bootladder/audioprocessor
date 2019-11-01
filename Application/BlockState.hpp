#ifndef __BLOCKSTATE_HPP__
#define __BLOCKSTATE_HPP__

enum BlockParamIdentifier_t{
  PARAM_0 = 0,
};
class BlockState {
  int param0;

public:
  void setParam(BlockParamIdentifier_t param, int value){
    (void)param;
    param0 = value;
  }
  int getParam(BlockParamIdentifier_t param){
    (void)param;
    return param0;
  }
};
#endif
