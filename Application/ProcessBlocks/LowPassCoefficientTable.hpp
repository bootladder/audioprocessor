#ifndef _LOWPASSCOEFFICIENTTABLE_H_
#define _LOWPASSCOEFFICIENTTABLE_H_

#include "CoefficientTable.hpp"
#include "LowPassFilterCoefficients.hpp"

class LowPassCoefficientTable : public CoefficientTable{

  static constexpr struct LowPassFilterCoefficients actualCoeffs = LowPassFilterCoefficients();

public:
  LowPassCoefficientTable(){
  }
  sample_t * lookupCutoffFrequency(int freq){
    (void)freq;
    //return actualCoeffs.arr[10];
    return 0;
  }
  int getNumTaps(){
    return 500;
  }
};



#endif /* _LOWPASSCOEFFICIENTTABLE_H_ */
