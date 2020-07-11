#include "MIDIReceiver.hpp"
#include "SamplingTypes.h"

class LambdaEnvelopeFollower : public MIDIReceiver {

  const char *name;
  std::function<void(int)> lambda;
  std::function<int(void)> envelopeGetterLambda;
  sample_t gain;

public:
  LambdaEnvelopeFollower(const char *name) : name(name), gain(1.0) {
  }

  void setLambda(std::function<void(int)> l) {
    lambda = l;
  }

  void setEnvelopeGetterLambda(std::function<int(void)> l){
    envelopeGetterLambda = l;
  }

  void setGain(sample_t g){
    gain = g;
  }
  sample_t getGain(){
    return gain;
  }

  void tickCallback() {

    // get the value of the envelope
    // modify according to scale parameters
    sample_t envelopeMag = envelopeGetterLambda();
    sample_t scaled = envelopeMag * gain;
    lambda((int)scaled);
  }


  void setMIDIParameter(BlockParamIdentifier_t id, int value){

    if(PARAM_0 == id){
      gain = ((sample_t)value / (sample_t)128.0 ) * (sample_t)2.0;
    }
    else if(PARAM_1 == id){
    }
  }

};