#include "ProcessBlock.hpp"

extern "C"{
#include "SerialLogger.h"
#include "tinyprintf.h"
}

typedef float (* GetFrequencyFunction)(void);
typedef sample_t (* GetAmplitudeFunction)(void);

typedef enum {
  OSCILLATOR_SQUARE,
} OscillatorType;

class OscillatorBlock : public ProcessBlock {

private:
  GetFrequencyFunction getFrequencyFunction;
  GetAmplitudeFunction getAmplitudeFunction;
  OscillatorType oscillatorType;
  uint32_t period_samples;
  uint32_t index;
  bool muted;

  sample_t oscillator_square(uint32_t index, uint32_t period){
    if(index < period/2)
      return 1;
    else
      return -1;
  }

public:
  OscillatorBlock(const char * name, uint32_t size, OscillatorType t, GetFrequencyFunction ff, GetAmplitudeFunction fa) :
    ProcessBlock(name, size){
    getFrequencyFunction = ff;
    getAmplitudeFunction = fa;
    oscillatorType = t;
    index = 0;
    muted = false;
  }

  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)value;  //value is always 127.  The ID is note on or off
    //PARAM_0 is ON, PARAM_1 is OFF
    if(id == PARAM_0){
      muted = false;
    }
    if(id == PARAM_1){
      muted = true;
    }

    static char str[100];
    int size = tfp_snprintf(str,100, "%s, Oscillator, %d\n", name, id);
    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
  }


  void process(sample_t * samplesToProcess)
  {
    (void)samplesToProcess; //oscillators do not depend on input

    if(muted){
      muteOutputBuffer();
      return;
    }

    sample_t freq = getFrequencyFunction();
    //sample_t freq_samples = freq/48000.0; //HARD CODED SAMPLE RATE
    //period_samples = (uint32_t) (1.0/freq_samples);
    period_samples = (uint32_t) (48000.0/freq);

    for(uint32_t i=0; i<num_samples; i++){
      outputBuffer[i] = getAmplitudeFunction() * oscillator_square(index, period_samples);
      index++;
      if(index >= period_samples)
        index = 0;
    }
  }

  void muteOutputBuffer(){
    for(uint32_t i=0; i<num_samples; i++){
      outputBuffer[i] = 0;
    }
  }
};
