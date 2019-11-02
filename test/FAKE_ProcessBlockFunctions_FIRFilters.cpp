#include <stdint.h>
#include "SamplingTypes.hpp"
#include "BlockState.hpp"

void ProcessBlockFunctions_FIRLowPass(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  (void)state;
  (void)in;
  (void)out;
  (void)size;
}
void ProcessBlockFunctions_FIRLowPass_CalculateCoefficients(uint32_t cutoff)
{
  (void)cutoff;
}
