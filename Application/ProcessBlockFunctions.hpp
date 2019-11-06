#ifndef __PROCESSBLOCKFUNCTIONS_HPP__
#define __PROCESSBLOCKFUNCTIONS_HPP__
#include <stdint.h>

#include "SamplingTypes.hpp"
#include "BlockState.hpp"

void ProcessBlockFunctions_Identity(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_Gain2X(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_GainParameterized(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_Attenuation(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_ClippingDistortion(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_Mixer(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
void ProcessBlockFunctions_Delay(BlockState * state, sample_t * in, sample_t * out, uint32_t size);
#endif
