#ifndef _LOWPASSCOEFFICIENTTABLE_H_
#define _LOWPASSCOEFFICIENTTABLE_H_

#include "CoefficientTable.hpp"
#include "LowPassFilterCoefficients.hpp"


constexpr LowPassFilterCoefficients lowPassFilterCoefficients;

class LowPassCoefficientTable : public CoefficientTable{

  //static constexpr struct LowPassFilterCoefficients actualCoeffs = LowPassFilterCoefficients();

public:
  LowPassCoefficientTable(){
  }
  sample_t * lookupCutoffFrequency(int freq){
    (void)freq;

    const float * blah = lowPassFilterCoefficients.arr[100];
    return (sample_t *) blah;
  }
  int getNumTaps(){
    return 512;
  }
};



#endif /* _LOWPASSCOEFFICIENTTABLE_H_ */
