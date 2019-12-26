#ifndef __FIRBLOCK_HPP__
#define __FIRBLOCK_HPP__

#include "ProcessBlock.hpp"
#include "FIRCoefficients.hpp"
#include "FIRProcessor.hpp"
#include "CoefficientTable.hpp"


class FIRBlock : public ProcessBlock{

  float * filter_coefficients;
  FIRCoefficients * coeffs;
  FIRProcessor & firProcessor;
  CoefficientTable & coefficientTable;

public:
  FIRBlock(const char * name, uint32_t size, FIRProcessor & cfirp, CoefficientTable & table);

  void process(sample_t * samplesToProcess);
  void setCoeffs(FIRCoefficients * c) { coeffs = c;}
  void assignCoefficientArray(uint8_t midivalue);
  void setMIDIParameter(BlockParamIdentifier_t id, int value);
};


#endif
