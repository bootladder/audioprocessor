#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "IIRBlock.hpp"


#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];

void loadBufferWithImpulse(sample_t * buf, int size, sample_t mag){
  for(int i=0; i<size ; i++){
    buf[i] = 0.0;
  }
  buf[0] = mag;
}

void loadBufferWithZeros(sample_t * buf, int size){
  for(int i=0; i<size ; i++){
    buf[i] = 0.0;
  }
}

//this comes straight out of the DAFX book

//xh(n) = x(n) − a1xh(n − 1) − a2xh(n − 2)

//xh(0) = x(0) − a1xh(0 − 1) − a2xh(0 − 2) = x(0) - a1*0 - a2*0 = 1
//xh(1) = x(1) − a1xh(1 − 1) − a2xh(1 − 2) = 0 - a1*1 - a2*0 = -a1
//xh(2) = x(2) − a1xh(2 − 1) − a2xh(2 − 2) = 0 - a1*(-a1) - a2(1) = a1*a1 - a2
//xh(3) = x(3) − a1xh(3 − 1) − a2xh(3 − 2) = 0 - a1*(a1*a1 - a2) - a2*(-a1) = -a1*a1*a1 + a1*a2 + a1*a2

//y(n) = b0xh(n) + b1xh(n − 1) + b2xh(n − 2)

//y(0) = b0*xh(0) + b1*xh(0 - 1) + b2*xh(0 - 2) = b0*1 = b0
//y(1) = b0*xh(1) + b1*xh(1 - 1) + b2*xh(1 - 2) = b0*(-a1) + b1*(1) = b0*(-a1) + b1
//y(2) = b0*xh(2) + b1*xh(2 - 1) + b2*xh(2 - 2) = b0*(a1*a1 - a2) + b1*(-a1) + b2*(1)
//y(3) = b0*xh(3) + b1*xh(3 - 1) + b2*xh(3 - 2) = b0*(-a1*a1*a1 + a1*a2 + a1*a2) + b1*(a1*a1 - a2) + b2*(-a1)

TEST(IIRBlock, impulseresponse)
{
  IIRBlock block("name", NUM_SAMPLES);

  sample_t a1 = 0.1;
  sample_t a2 = 0.2;
  sample_t b0 = 0.3;
  sample_t b1 = 0.4;
  sample_t b2 = 0.5;

  block.setCoefficient(IIR_COEFF_A1, a1);
  block.setCoefficient(IIR_COEFF_A2, a2);
  block.setCoefficient(IIR_COEFF_B0, b0);
  block.setCoefficient(IIR_COEFF_B1, b1);
  block.setCoefficient(IIR_COEFF_B2, b2);

  loadBufferWithImpulse(testBuf, NUM_SAMPLES, 1.0);


  block.process(testBuf);

  sample_t * out = block.getOutputBuffer();

  ASSERT_EQ(b0,out[0]);
  ASSERT_EQ(b0*-1*a1 + b1,out[1]);
  ASSERT_EQ(b0*(a1*a1 - a2) + b1*(-1*a1) + b2*(1), out[2]);
  ASSERT_EQ(b0*(-1*a1*a1*a1 + a1*a2 + a1*a2) + b1*(a1*a1 - a2) + b2*(-1*a1), out[3]);
}

TEST(IIRBlock, impulseresponse_magnitiude2)
{
  IIRBlock block("name", NUM_SAMPLES);

  sample_t a1 = 0.1;
  sample_t a2 = 0.2;
  sample_t b0 = 0.3;
  sample_t b1 = 0.4;
  sample_t b2 = 0.5;

  block.setCoefficient(IIR_COEFF_A1, a1);
  block.setCoefficient(IIR_COEFF_A2, a2);
  block.setCoefficient(IIR_COEFF_B0, b0);
  block.setCoefficient(IIR_COEFF_B1, b1);
  block.setCoefficient(IIR_COEFF_B2, b2);

  loadBufferWithImpulse(testBuf, NUM_SAMPLES, 2.0);


  block.process(testBuf);

  sample_t * out = block.getOutputBuffer();

  ASSERT_EQ(b0*2,out[0]);
}


TEST(IIRBlock, impulseresponse_doubleLength_isSameAsTwoSingleLengths)
{
  const int numSamplesForThisTest = 6;
  IIRBlock blocklong("long", numSamplesForThisTest*2);
  IIRBlock blockshort1("short1", numSamplesForThisTest);

  sample_t a1 = 0.1;
  sample_t a2 = 0.2;
  sample_t b0 = 0.3;
  sample_t b1 = 0.4;
  sample_t b2 = 0.5;

  IIRBlock * blocks[] = {&blockshort1, &blocklong};
  for(int i=0; i<2; i++){
    blocks[i]->setCoefficient(IIR_COEFF_A1, a1);
    blocks[i]->setCoefficient(IIR_COEFF_A2, a2);
    blocks[i]->setCoefficient(IIR_COEFF_B0, b0);
    blocks[i]->setCoefficient(IIR_COEFF_B1, b1);
    blocks[i]->setCoefficient(IIR_COEFF_B2, b2);
  }

  static sample_t testBufShort[numSamplesForThisTest];
  static sample_t testBufZeros[numSamplesForThisTest];
  static sample_t testBufLong[numSamplesForThisTest*2];

  loadBufferWithImpulse(testBufShort, numSamplesForThisTest, 1.0);
  loadBufferWithZeros(testBufZeros, numSamplesForThisTest);
  loadBufferWithImpulse(testBufLong, numSamplesForThisTest*2, 1.0);


  blocklong.process(testBufLong);
  sample_t * outlong = blocklong.getOutputBuffer();

  blockshort1.process(testBufShort);
  sample_t * outshort1 = blockshort1.getOutputBuffer();

  for(int i=0; i<numSamplesForThisTest; i++){
    ASSERT_EQ(outshort1[i], outlong[i]);
  }


  blockshort1.process(testBufZeros);
  sample_t * outshort2 = blockshort1.getOutputBuffer();

  for(int i=0; i<numSamplesForThisTest; i++){
    ASSERT_EQ(outshort2[i], outlong[numSamplesForThisTest+ i]);
  }
}

TEST(IIRBlock, setCutoffFrequency_checkCoeffs){

  IIRBlock block("name", NUM_SAMPLES);
  block.setCutoffFrequency(1000);
  block.setQ(1.0);

  sample_t a = 3.1415927 * 1000.0 /48000.0;
  sample_t K = tan(a);
  sample_t Q = 1.0;

  ASSERT_EQ((2*Q*(K*K-1)) / (K*K*Q+K+Q), block.getCoefficient(IIR_COEFF_A1));
  ASSERT_EQ((K*K*Q-K+Q)/(K*K*Q+K+Q), block.getCoefficient(IIR_COEFF_A2));

  ASSERT_EQ( K / (K*K*Q + K + Q), block.getCoefficient(IIR_COEFF_B0));
  ASSERT_EQ(0.0, block.getCoefficient(IIR_COEFF_B1));
  ASSERT_EQ( -1* K / (K*K*Q + K + Q), block.getCoefficient(IIR_COEFF_B2));
}


TEST(IIRBlock, setMIDIParameter_Cutoff_Works){

  IIRBlock block("name", NUM_SAMPLES);

  block.setMIDIParameter(PARAM_0, 100);
  block.setMIDIParameter(PARAM_1, 100);
  
  //ASSERT_EQ(2000.0, block.getCutoffFrequency());
  
  ASSERT_EQ(10.0, block.getQ());
}

TEST(IIRBlock, setDeltaCutoffFrequency) {
  IIRBlock block("name", NUM_SAMPLES);
  block.setCutoffFrequency(1000);
  block.setDeltaCutoffFrequency(50);

  sample_t newFreq = block.getCutoffFrequency();

  ASSERT_EQ(1050, newFreq);
}


TEST(IIRBlock, setDeltaCutoffFrequency_setTwice_setsToSameValue) {
  IIRBlock block("name", NUM_SAMPLES);
  block.setCutoffFrequency(1000);
  block.setDeltaCutoffFrequency(50);
  block.setDeltaCutoffFrequency(50);

  sample_t newFreq = block.getCutoffFrequency();

  ASSERT_EQ(1050, newFreq);
}

// only set the base, not the actual
TEST(IIRBlock, setCutoffFrequency_preservesDelta) {
  IIRBlock block("name", NUM_SAMPLES);
  block.setCutoffFrequency(1000);
  block.setDeltaCutoffFrequency(50);
  block.setCutoffFrequency(1000);

  sample_t newFreq = block.getCutoffFrequency();

  ASSERT_EQ(1050, newFreq);
}

TEST(IIRBlock, process_preserves_signal_RMS)
{
  IIRBlock block("name", NUM_SAMPLES);
  block.setCutoffFrequency(1000);

  for(int i=0; i<NUM_SAMPLES; i++){

  }
  block.process(testBuf);
}