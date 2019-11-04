#include "ProcessBlockFunctions_FIRFilters.hpp"

extern "C"{
#include "SerialLogger.h"
}



void ProcessBlockFunctions_FIRLowPass_CalculateCoefficients(uint32_t cutoff)
{
  SerialLogger_PrintLiteralString("Bad1");
}


void ProcessBlockFunctions_FIRLowPass(BlockState * state, sample_t * in, sample_t * out, uint32_t size)
{
  SerialLogger_PrintLiteralString("Bad2");
}

