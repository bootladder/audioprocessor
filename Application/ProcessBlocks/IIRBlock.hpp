#ifndef __IIRBLOCK_HPP__
#define __IIRBLOCK_HPP__


// Currently this assumes we are a BAND PASS FILTER

#include "ProcessBlock.hpp"
#include <memory>

extern "C"{
#include "SerialLogger.h"
#include "tinyprintf.h"
}

typedef enum {
              IIR_COEFF_A1,
              IIR_COEFF_A2,
              IIR_COEFF_B0,
              IIR_COEFF_B1,
              IIR_COEFF_B2
} IIRCoefficientID;

class IIRBlock : public ProcessBlock{

  sample_t a1;
  sample_t a2;
  sample_t b0;
  sample_t b1;
  sample_t b2;

  sample_t Q;
  sample_t baseCutoffFreq;  //actual cutoff can be base+delta
  sample_t delta;

  sample_t cutoffFreq;
  std::unique_ptr<sample_t[]> xh;
  sample_t last_xh;

  sample_t secondToLast_xh;
public:

  IIRBlock(const char * name, uint32_t size)
  : ProcessBlock(name, size)
  {
    cutoffFreq = 1500.0;
    baseCutoffFreq = 0.0;
    delta = 0.0;
    Q = 0.9;
    updateCoeffs();
    xh  = std::make_unique<sample_t[]>(num_samples);
    last_xh = 0.0;
    secondToLast_xh = 0.0;
  }

  void process(const sample_t *samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }

    //xh(n) = x(n) − a1xh(n − 1) − a2xh(n − 2)
    //xh(0) = x(0) − a1xh(0 − 1) − a2xh(0 − 2) = 1 - a1*0 - a2*0 = 1
    //xh(1) = x(1) − a1xh(1 − 1) − a2xh(1 − 2) = 0 - a1*1 - a2*0 = -a1

    xh[0] = inputBuffer[0] - (a1*last_xh) - (a2*secondToLast_xh); //inputBuffer[0];
    xh[1] = inputBuffer[1] - (a1*xh[0]) - (a2*last_xh);
    for(uint32_t i=2; i<num_samples; i++){
      xh[i] = inputBuffer[i] - (a1*xh[i-1]) - (a2*xh[i-2]);
    }

    //y(n) = b0xh(n) + b1xh(n − 1) + b2xh(n − 2)
    //y(0) = b0*xh(0) + b1*xh(0 - 1) + b2*xh(0 - 2) = b0*1 = b0
    //y(1) = b0*xh(1) + b1*xh(1 - 1) + b2*xh(1 - 2) = b0*(-a1) + b1*(1) = b0*(-a1) + b1

    outputBuffer[0] = b0*xh[0] + b1*last_xh + b2*secondToLast_xh;
    outputBuffer[1] = b0*xh[1] + b1*xh[0] + b2*last_xh;

    for(uint32_t i=2; i<num_samples; i++){
      outputBuffer[i] = b0*xh[i] + b1*xh[i-1] + b2*xh[i-2];
    }

    last_xh = xh[num_samples-1];
    secondToLast_xh = xh[num_samples-2];
  }

  void setCoefficient(IIRCoefficientID id, sample_t value){
    switch(id){
      case IIR_COEFF_A1: a1 = value; break;
      case IIR_COEFF_A2: a2 = value; break;
      case IIR_COEFF_B0: b0 = value; break;
      case IIR_COEFF_B1: b1 = value; break;
      case IIR_COEFF_B2: b2 = value; break;
    }
  }

  sample_t getCoefficient(IIRCoefficientID id){
    switch(id){
      case IIR_COEFF_A1: return a1;
      case IIR_COEFF_A2: return a2;
      case IIR_COEFF_B0: return b0;
      case IIR_COEFF_B1: return b1;
      case IIR_COEFF_B2: return b2;
    }
    return 0.0;
  }

  void setCutoffFrequency(int freq){
    baseCutoffFreq = freq;
    cutoffFreq = freq + delta;
    updateCoeffs();
  }

  void setDeltaCutoffFrequency(int delta){
    this->delta = (sample_t)delta;
    cutoffFreq = baseCutoffFreq + delta;
    updateCoeffs();
  }

  sample_t getCutoffFrequency(){ return cutoffFreq;  }
  void setQ(sample_t value){
    Q = value;
    updateCoeffs();
  }

  sample_t getQ(){ return Q;  }


  //K = tan(π*fc/fS)
  void updateCoeffs(){
    sample_t a = 3.1415927 * cutoffFreq /48000.0;
    sample_t K = tan(a);

    // from the DAFX book
    a1 = (2*Q*(K*K-1)) / (K*K*Q+K+Q);
    a2 = (K*K*Q-K+Q)/(K*K*Q+K+Q);
    b0 = ( K / (K*K*Q + K + Q));
    b1 = 0.0;
    b2 = -1* K / (K*K*Q + K + Q);
  }


  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    if(id == PARAM_0){

      int newCutoff = 200 + (value*8);

      static char str[100];
      int size = tfp_snprintf(str,100, "%s, Cutoff(Hz), %d\n", name, newCutoff);
      SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
      setCutoffFrequency(newCutoff);
    }
    else{
      sample_t newQ = ((float)value)/10.0;
      static char str[100];
      int size = tfp_snprintf(str,100, "%s, Q, %f\n", name, newQ);
      SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
      setQ(newQ);
    }
  }
};


#endif
