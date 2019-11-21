#include "FIRBlock.hpp"

#include "BSP_Audio_Buffer_Interface.h"

#include "constexpr_arm_sin_f32.hpp"


extern "C"{
#include "MemoryLogger.h"
}

#define SAMPLE_FREQUENCY 48000.0
#define MAX_BLOCK_SIZE 2048
#define MAX_NUM_TAPS 512



#include "LowPassFilterCoefficients.hpp"

static constexpr struct LowPassFilterCoefficients low_pass_filter_coefficients = LowPassFilterCoefficients();


FIRBlock::FIRBlock(const char * name, uint32_t size, FIRProcessor & cfirp)
: ProcessBlock(name, size),
  firProcessor(cfirp)
{

  filter_coefficients = new float[MAX_NUM_TAPS];
  firStateF32 = new float[MAX_BLOCK_SIZE + MAX_NUM_TAPS - 1];
  arm_fir_init_f32(&S, MAX_NUM_TAPS, (float*)low_pass_filter_coefficients.arr[40],
                   &firStateF32[0], MY_PROCESSING_BUFFER_SIZE_SAMPLES); //blocksize

  MemoryLogger_LogStringLn("FIR Coeffs calcualtd");
}

void FIRBlock::assignCoefficientArray(uint8_t midivalue)
{
  S.pCoeffs = (float * )low_pass_filter_coefficients.arr[midivalue];
}

//overriding setMIDIParameter to update coefficients here
//now there's only 1 parameter but there could be other parameters in this block
void FIRBlock::setMIDIParameter(BlockParamIdentifier_t id, int value){
  (void)id;
  assignCoefficientArray(value);
  static char str[100];
  int size = tfp_snprintf(str,100, "%s, Cutoff(Hz), %d\n", name, midi_value_to_cutoff(value));
  SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
}


void FIRBlock::process(sample_t * samplesToProcess){
  for(uint32_t i=0; i<num_samples; i++){
    inputBuffer[i] = samplesToProcess[i];
  }
  arm_fir_f32(&S, inputBuffer, outputBuffer, num_samples);
}
