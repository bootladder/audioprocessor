#include <iostream>
using namespace std;
#include "gtest/gtest.h"

#include "DelayBlock.hpp"

#define NUM_SAMPLES 1024

TEST(ProcessBlockDelay, SetMIDIParameter_SetsDelayNumberOfSamples)
{
  // Set Delay parameter
  // Formula should be : (X/128) * 100ms
  // so 64 ==> 50ms
  // Actual delay in samples D = (X/128) * 100ms * (1sec/1000ms) * (SAMPLERATE samples/1sec)

  float delayNumSamples_float = (64.0/128.0) * 100.0 * (1.0/1000.0) * (48000.0);
  int delayNumSamples = (int) delayNumSamples_float;

  DelayBlock delayBlock("name", NUM_SAMPLES);
  delayBlock.setMIDIParameter(PARAM_0, 64);

  ASSERT_EQ(delayNumSamples, delayBlock.getDelayNumSamples());
}

TEST(DelayBuffer, FillUpHalfWay)
{
  #define TEST_DELAY_BUFFER_SIZE 16
  DelayBuffer delayBuffer(TEST_DELAY_BUFFER_SIZE);

  //fill up half way
  for(int i=0; i<TEST_DELAY_BUFFER_SIZE/2; i++){
    sample_t samp = i;
    delayBuffer.insertSample(samp);
  }

  ASSERT_EQ(delayBuffer.getDelayedSample(0), (TEST_DELAY_BUFFER_SIZE / 2) - 1);
  ASSERT_EQ(delayBuffer.getDelayedSample(1), (TEST_DELAY_BUFFER_SIZE / 2) - 2);
  ASSERT_EQ(delayBuffer.getDelayedSample(2), (TEST_DELAY_BUFFER_SIZE / 2) - 3);
  ASSERT_EQ(delayBuffer.getDelayedSample(3), (TEST_DELAY_BUFFER_SIZE / 2) - 4);
  ASSERT_EQ(delayBuffer.getDelayedSample(4), (TEST_DELAY_BUFFER_SIZE / 2) - 5);
}
TEST(DelayBuffer, FillUpFull)
{
  #define TEST_DELAY_BUFFER_SIZE 16
  DelayBuffer delayBuffer(TEST_DELAY_BUFFER_SIZE);

  //fill up whole way
  for(int i=0; i<TEST_DELAY_BUFFER_SIZE; i++){
    sample_t samp = i;
    delayBuffer.insertSample(samp);
  }

  ASSERT_EQ(delayBuffer.getDelayedSample(0), (TEST_DELAY_BUFFER_SIZE) - 1);
  ASSERT_EQ(delayBuffer.getDelayedSample(1), (TEST_DELAY_BUFFER_SIZE) - 2);
  ASSERT_EQ(delayBuffer.getDelayedSample(2), (TEST_DELAY_BUFFER_SIZE) - 3);
  ASSERT_EQ(delayBuffer.getDelayedSample(3), (TEST_DELAY_BUFFER_SIZE) - 4);
  ASSERT_EQ(delayBuffer.getDelayedSample(4), (TEST_DELAY_BUFFER_SIZE) - 5);

}

TEST(DelayBuffer, FillUpFullPlusACoupleMore)
{
  #define TEST_DELAY_BUFFER_SIZE 16
  DelayBuffer delayBuffer(TEST_DELAY_BUFFER_SIZE);

  //fill up whole way
  for(int i=0; i<TEST_DELAY_BUFFER_SIZE; i++){
    sample_t samp = i;
    delayBuffer.insertSample(samp);
  }

  delayBuffer.insertSample((sample_t) 370);
  delayBuffer.insertSample((sample_t) 371);
  delayBuffer.insertSample((sample_t) 372);

  ASSERT_EQ(delayBuffer.getDelayedSample(0), 372);
  ASSERT_EQ(delayBuffer.getDelayedSample(1), 371);
  ASSERT_EQ(delayBuffer.getDelayedSample(2), 370);
}

TEST(DelayBuffer, FillUpLargeAmount)
{
  #define TEST_DELAY_BUFFER_SIZE 16
  DelayBuffer delayBuffer(TEST_DELAY_BUFFER_SIZE);

  //fill up a large amoutn
  int largeNumber = (TEST_DELAY_BUFFER_SIZE * 7) + 376;
  for(int i=0; i < largeNumber ; i++){
    sample_t samp = i;
    delayBuffer.insertSample(samp);
  }

  ASSERT_EQ(delayBuffer.getDelayedSample(0), largeNumber - 1);
  ASSERT_EQ(delayBuffer.getDelayedSample(1), largeNumber - 2);
  ASSERT_EQ(delayBuffer.getDelayedSample(2), largeNumber - 3);
  ASSERT_EQ(delayBuffer.getDelayedSample(3), largeNumber - 4);
  ASSERT_EQ(delayBuffer.getDelayedSample(4), largeNumber - 5);
}



// Currently the delay is being implemented completely heuristically
// Will test later

//TEST(ProcessBlockDelay, DelaysCorrectNumberOfSamples)
//{
//  DelayBlock delayBlock("name", NUM_SAMPLES);
//
//  //output is equal to the input plus the delayed input
//  static sample_t inputBuf[NUM_SAMPLES*8];
//
//  for(int i=0; i<NUM_SAMPLES * 8; i++){
//    inputBuf[i] = (sample_t)i;
//  }
//
//  delayBlock.setMIDIParameter(PARAM_0, 64);
//  int delayNumSamples = delayBlock.getDelayNumSamples();
//
//  cout << "NUMBER OF DELAY SAMPLES: " << delayNumSamples << endl;
//
//
//  delayBlock.process(inputBuf + (NUM_SAMPLES * 0) );
//  delayBlock.process(inputBuf + (NUM_SAMPLES * 1) );
//  delayBlock.process(inputBuf + (NUM_SAMPLES * 2) );
//  delayBlock.process(inputBuf + (NUM_SAMPLES * 3) );
//  delayBlock.process(inputBuf + (NUM_SAMPLES * 4) );
//  delayBlock.process(inputBuf + (NUM_SAMPLES * 5) );
//  delayBlock.process(inputBuf + (NUM_SAMPLES * 6) );
//  delayBlock.process(inputBuf + (NUM_SAMPLES * 7) );
//
//  sample_t * out = delayBlock.getOutputBuffer();
//
//  sample_t * lastPartOfInputBuf = &inputBuf[NUM_SAMPLES*7];
//
//  for(int i=0; i<NUM_SAMPLES; i++){
//    //cout << "i : " << i << " OUT[i] : " << out[i] << " lastPartOfInputBuf[i] : " << lastPartOfInputBuf[i] << endl;;
//    ASSERT_EQ(out[i],  lastPartOfInputBuf[i] + lastPartOfInputBuf[i - delayNumSamples - 1]);
//  }
//}
