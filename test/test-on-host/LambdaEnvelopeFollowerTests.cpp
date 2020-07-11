#include <iostream>
using namespace std;
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using namespace testing;

#include "LambdaEnvelopeFollower.hpp"
#include "IIRBlock.hpp"


TEST(LambdaEnvelopeFollower, applies_gain_and_callsLambda)
{
  LambdaEnvelopeFollower envelopeFollower("name");

  IIRBlock iir1("iir1", 1024);
  auto lambda = [block = &iir1](int f){block->setDeltaCutoffFrequency(f);};
  auto envelopeGetterLambda = [](){return 400;};

  envelopeFollower.setLambda(lambda);
  envelopeFollower.setEnvelopeGetterLambda(envelopeGetterLambda);
  envelopeFollower.setGain(0.5);

  envelopeFollower.tickCallback();

  ASSERT_EQ(iir1.getCutoffFrequency(), 200);
}



TEST(LambdaEnvelopeFollower, midi_param0_is_gain) {
  LambdaEnvelopeFollower envelopeFollower("name");
  envelopeFollower.setGain(0.2);
  envelopeFollower.setMIDIParameter(PARAM_0, 120);
  ASSERT_NE(0.2, envelopeFollower.getGain());
}

