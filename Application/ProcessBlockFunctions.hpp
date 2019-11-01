#ifndef __PROCESSBLOCKFUNCTIONS_HPP__
#define __PROCESSBLOCKFUNCTIONS_HPP__
#include <stdint.h>

#include "BlockState.hpp"

void ProcessBlockFunctions_Identity(BlockState * state, int16_t * in, int16_t * out, uint32_t size);
void ProcessBlockFunctions_Gain2X(BlockState * state, int16_t * in, int16_t * out, uint32_t size);
void ProcessBlockFunctions_GainParameterized(BlockState * state, int16_t * in, int16_t * out, uint32_t size);
#endif
