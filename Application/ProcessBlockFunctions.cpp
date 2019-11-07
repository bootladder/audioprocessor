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

  //params are midi values, 8bits.
  //scale to an float, 0 to 8.0
  int gain = state->getParam(PARAM_0);
  float gain_float = (float)gain * 8.0 /128.0;
  for(uint32_t i=0; i<size; i++){
    out[i] = in[i] * gain_float;
  }
}

void ProcessBlockFunctions_Attenuation(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  //params are midi values, up to 127
  //scale to an float, 0 to 2.0
  int gain = state->getParam(PARAM_0);
  float gain_float = (float)gain * 2.0 /128.0;
  for(uint32_t i=0; i<size; i++){
    out[i] = in[i] * gain_float;
  }
}

void ProcessBlockFunctions_ClippingDistortion(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  (void)state;
  //clip half way, both ends
  sample_t max = (sample_t) 0x4000/2;
  sample_t min = (sample_t) -(0x4000/2);

  for(uint32_t i=0; i<size; i++){
    if(in[i] > max)
      out[i] = max;
    else if(in[i] < min)
      out[i] = min;
    else
      out[i] = in[i];
  }
}

void ProcessBlockFunctions_Mixer(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  (void)state;

  for(uint32_t i=0; i<size; i++){
    out[i] = out[i] + in[i];
  }
}

void ProcessBlockFunctions_Delay(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  (void)state;

  //don't use the static size here
  static sample_t lastbuf0[1024];
  static sample_t lastbuf1[1024];
  static sample_t lastbuf2[1024];
  static sample_t lastbuf3[1024];
  static sample_t lastbuf4[1024];
  static sample_t lastbuf5[1024];
  static sample_t lastbuf6[1024];

  //add delayed input to input
  for(uint32_t i=0; i<size; i++){
    out[i] = in[i] + lastbuf3[i];
  }


  //update delay lines
  for(uint32_t i=0; i<size; i++){
    lastbuf6[i] = lastbuf5[i];
    lastbuf5[i] = lastbuf4[i];
    lastbuf4[i] = lastbuf3[i];
    lastbuf3[i] = lastbuf2[i];
    lastbuf2[i] = lastbuf1[i];
    lastbuf1[i] = lastbuf0[i];
    lastbuf0[i] = in[i];
  }
}
