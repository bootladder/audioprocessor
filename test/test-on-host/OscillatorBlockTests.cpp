#include <iostream>
using namespace std;
#include "gtest/gtest.h"

#include "OscillatorBlock.hpp"

#define NUM_SAMPLES 1024
static sample_t testBuf[NUM_SAMPLES];


sample_t return_constant_frequency(void){
  return 1000.0;
}

sample_t return_constant_amplitude(void){
  return 7000.0;
}

TEST(OscillatorBlock, squarewave)
{
  OscillatorBlock block("name", NUM_SAMPLES, OSCILLATOR_SQUARE, return_constant_frequency, return_constant_amplitude);

  block.process(testBuf);
  sample_t * out = block.getOutputBuffer();

  //check for some samples equal to amplitude
  bool found;
  found = false;
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    if(out[i] == return_constant_amplitude())
      found = true;
  }
  ASSERT_TRUE(found);

  //check for some samples equal to NEGATIVE amplitude
  found = false;
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    if(out[i] == -1 * return_constant_amplitude())
      found = true;
  }
  ASSERT_TRUE(found);

  //check for toggling every 1000Hz == 48 samples === SAMPLERATE/10
  int firstIndex = 0;
  int secondIndex = 0;
  for(uint32_t i=0; i<NUM_SAMPLES; i++){

    if(out[i] == return_constant_amplitude() &&
       out[i+1] == -1*return_constant_amplitude() ){
      firstIndex = i;
      break;
    }
  }

  for(uint32_t i=firstIndex+1; i<NUM_SAMPLES; i++){
      if(out[i] == return_constant_amplitude() &&
         out[i+1] == -1*return_constant_amplitude() ){
      secondIndex = i;
      break;
    }
  }

  ASSERT_EQ(secondIndex - firstIndex , 48);
}


//we don't want the square wave starting from index zero.
//the oscillator needs to keep its phase internally
TEST(OscillatorBlock, squarewave_multiplecalls_is_still_in_phase)
{
  OscillatorBlock block("name", NUM_SAMPLES, OSCILLATOR_SQUARE, return_constant_frequency, return_constant_amplitude);

  sample_t bigbuf[NUM_SAMPLES*4];
  sample_t * out;

  block.process(testBuf);
  out = block.getOutputBuffer();
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    bigbuf[i + 0] = out[i];
  }

  block.process(testBuf);
  out = block.getOutputBuffer();
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    bigbuf[i + 1024] = out[i];
  }

  block.process(testBuf);
  out = block.getOutputBuffer();
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    bigbuf[i + 2048] = out[i];
  }

  block.process(testBuf);
  out = block.getOutputBuffer();
  for(uint32_t i=0; i<NUM_SAMPLES; i++){
    bigbuf[i + 3072] = out[i];
  }


  //Find the first falling edge of the square wave
  int firstIndex = 0;
  for(uint32_t i=0; i<NUM_SAMPLES; i++){

    if(bigbuf[i] == return_constant_amplitude() &&
       bigbuf[i+1] == -1*return_constant_amplitude() ){
      firstIndex = i;
      break;
    }
  }


  // the period is 48 samples, so check for... 50 periods later
  // this index should also be a falling edge
  int somelaterindex = firstIndex + (48*50);

  ASSERT_EQ(bigbuf[somelaterindex], return_constant_amplitude());
  ASSERT_EQ(bigbuf[somelaterindex+1], -1*return_constant_amplitude());
}
