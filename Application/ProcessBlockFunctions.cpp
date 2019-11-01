#include "ProcessBlockFunctions.hpp"

void ProcessBlockFunctions_Identity(BlockState * state, int16_t * in, int16_t * out, uint32_t size)
{
  (void) state;(void) in; (void) out; (void) size;
}

void ProcessBlockFunctions_Gain2X(BlockState * state, int16_t * in, int16_t * out, uint32_t size)
{
  (void) state;
  for(uint32_t i=0; i<size; i++){
    out[i] = in[i] * 2;
  }
}

void ProcessBlockFunctions_GainParameterized(BlockState * state, int16_t * in, int16_t * out, uint32_t size)
{
  int gain = state->getParam(PARAM_0);
  for(uint32_t i=0; i<size; i++){
    out[i] = in[i] * gain;
  }
}

