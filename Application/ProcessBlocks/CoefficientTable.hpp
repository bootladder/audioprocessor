#ifndef _COEFFICIENTTABLE_H_
#define _COEFFICIENTTABLE_H_

#include "SamplingTypes.h"

class CoefficientTable {
public:
  CoefficientTable (){
  }

  virtual sample_t * lookupCutoffFrequency(int freq) = 0;
};


class FakeCoefficientTable : public CoefficientTable {

public:
  FakeCoefficientTable():CoefficientTable(){}
  sample_t * lookupCutoffFrequency(int freq){
    (void)freq;
    return 0;
  }
};

#endif /* _COEFFICIENTTABLE_H_ */
