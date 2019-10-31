#include "DummyProcessFunctions.hpp"

#include <iostream>
using namespace std;

int16_t * DummyProcessFunctions_noop(int16_t * buf, uint32_t num_samples)
{
  (void)num_samples;
  return buf;
}

int16_t * DummyProcessFunctions_half_volume(int16_t * buf, uint32_t num_samples)
{
  for(uint32_t i=0; i<num_samples; i++){
    buf[i] = buf[i] / 2;
  }
  return buf;
}

int16_t * DummyProcessFunctions_add_1(int16_t * buf, uint32_t num_samples)
{
  for(uint32_t i=0; i<num_samples; i++){
    buf[i] = buf[i] + 1;
  }
  return buf;
}

int16_t * DummyProcessFunctions_add_2(int16_t * buf, uint32_t num_samples)
{
  for(uint32_t i=0; i<num_samples; i++){
    buf[i] = buf[i] + 2;
  }
  return buf;
}


static int16_t * _set_constant(int c, int16_t * buf, uint32_t num_samples){
  for(uint32_t i=0; i<num_samples; i++){
    buf[i] = c;
  }
  return buf;
}
int16_t * DummyProcessFunctions_Constant_5(int16_t * buf, uint32_t num_samples)
{
  return _set_constant(5, buf, num_samples);
}

int16_t * DummyProcessFunctions_Constant_7(int16_t * buf, uint32_t num_samples)
{
  return _set_constant(7, buf, num_samples);
}

int16_t * DummyProcessFunctions_mixer_addition(int16_t * buf, uint32_t num_samples)
{
  cout << "wat input: " << buf[0] << endl;

  static int16_t stateBuf[3000];
  for(uint32_t i=0; i<num_samples; i++){
    stateBuf[i] = stateBuf[i] + buf[i];
  }

  cout << "wat stateBuf " << stateBuf[0] << endl;
  return stateBuf;
}
