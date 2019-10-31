#include "DummyProcessFunctions.hpp"

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
