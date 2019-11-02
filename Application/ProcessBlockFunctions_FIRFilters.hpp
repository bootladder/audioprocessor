#include <stdint.h>
#include "BlockState.hpp"

void ProcessBlockFunctions_FIRLowPass(BlockState * state, int16_t * in, int16_t * out, uint32_t size);
void ProcessBlockFunctions_FIRLowPass_CalculateCoefficients(uint32_t cutoff);