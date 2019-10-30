#include "DummyProcessFunctions.hpp"

int16_t * DummyProcessFunctions_half_volume(int16_t * buf, uint32_t num_samples)
{
  for(uint32_t i=0; i<num_samples; i++){
    buf[i] = buf[i] / 2;
  }
  return buf;
}
