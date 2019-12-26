#include "FIRBlock.hpp"

#include "BSP_Audio_Buffer_Interface.h"


extern "C"{
#include "MemoryLogger.h"
#include "tinyprintf.h"
}

#define MAX_BLOCK_SIZE 2048




FIRBlock::FIRBlock(const char * name, uint32_t size, FIRProcessor & cfirp)
: ProcessBlock(name, size),
  firProcessor(cfirp)
{

  MemoryLogger_LogStringLn("FIR Coeffs calcualtd");
}

void FIRBlock::assignCoefficientArray(uint8_t midivalue)
{
  (void)midivalue;
}

//overriding setMIDIParameter to update coefficients here
//now there's only 1 parameter but there could be other parameters in this block
void FIRBlock::setMIDIParameter(BlockParamIdentifier_t id, int value){
  (void)id;
  assignCoefficientArray(value);
}


void FIRBlock::process(sample_t * samplesToProcess){
  for(uint32_t i=0; i<num_samples; i++){
    inputBuffer[i] = samplesToProcess[i];
  }

  firProcessor.calculate(inputBuffer, outputBuffer, *coeffs, num_samples);
}
