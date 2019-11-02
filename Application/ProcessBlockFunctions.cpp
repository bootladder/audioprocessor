#include "ProcessBlockFunctions.hpp"

void ProcessBlockFunctions_Identity(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  (void) state;(void) in; (void) out; (void) size;
}

void ProcessBlockFunctions_Gain2X(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  (void) state;
  for(uint32_t i=0; i<size; i++){
    out[i] = in[i] * 2;
  }
}

void ProcessBlockFunctions_GainParameterized(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  int gain = state->getParam(PARAM_0);
  for(uint32_t i=0; i<size; i++){
    out[i] = in[i] * gain;
  }
}

void ProcessBlockFunctions_ClippingDistortion(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  //clip half way, both ends
  sample_t max = (sample_t) 0x8000/2;
  sample_t min = (sample_t) -(0x8000/2);

  for(uint32_t i=0; i<size; i++){
    if(in[i] > max)
      out[i] = max;
    else if(in[i] < min)
      out[i] = min;
    else
      out[i] = in[i];
  }
}
