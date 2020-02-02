#include "FIRBlock.hpp"

#include "BSP_Audio_Buffer_Interface.h"


extern "C"{
#include "MemoryLogger.h"
#include "tinyprintf.h"
}

#define MAX_BLOCK_SIZE 2048



FIRBlock::FIRBlock(const char * name, uint32_t size, FIRProcessor & cfirp, CoefficientTable & table)
: ProcessBlock(name, size),
  firProcessor(cfirp),
  coefficientTable(table)
{
  coeffs = new FIRCoefficients(0,1);

}

void FIRBlock::assignCoefficientArray(uint8_t midivalue)
{
  (void)midivalue;
}

//still using midi values right now
void FIRBlock::setCutoffFrequency(int value){
  coeffs->coeffs = coefficientTable.lookupCutoffFrequency(value);
  coeffs->num_taps = coefficientTable.getNumTaps();
}

//overriding setMIDIParameter to update coefficients here
//now there's only 1 parameter but there could be other parameters in this block
void FIRBlock::setMIDIParameter(BlockParamIdentifier_t id, int value){
  (void)id;
  setCutoffFrequency(value);
}


void FIRBlock::process(sample_t * samplesToProcess){
  for(uint32_t i=0; i<num_samples; i++){
    inputBuffer[i] = samplesToProcess[i];
  }

  firProcessor.calculate(inputBuffer, outputBuffer, *coeffs, num_samples);
}
