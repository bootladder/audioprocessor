#ifndef __BLOCKSTATE_HPP__
#define __BLOCKSTATE_HPP__

enum BlockParamIdentifier_t{
  PARAM_0 = 0,
  PARAM_1 = 1,
};
class BlockState {
  int param0;
  int param1;

public:
  void setParam(BlockParamIdentifier_t param, int value){
    if(param == PARAM_0)
      param0 = value;
    if(param == PARAM_1)
      param1 = value;
  }
  int getParam(BlockParamIdentifier_t param){
    if(param == PARAM_0)
      return param0;
    if(param == PARAM_1)
      return param1;
    return 0;
  }
};
#endif
