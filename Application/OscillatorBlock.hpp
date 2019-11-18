#include "ProcessBlock.hpp"

typedef float (* GetFrequencyFunction)(void);
typedef sample_t (* GetAmplitudeFunction)(void);

typedef enum {
  OSCILLATOR_SQUARE,
} OscillatorType;

class OscillatorBlock : public RealProcessBlock {

private:
  GetFrequencyFunction getFrequencyFunction;
  GetAmplitudeFunction getAmplitudeFunction;
  OscillatorType oscillatorType;
  uint32_t period_samples;
  uint32_t index;

  sample_t oscillator_square(uint32_t index, uint32_t period){
    if(index < period/2)
      return 1;
    else
      return -1;
  }

public:
  OscillatorBlock(const char * name, uint32_t size, OscillatorType t, GetFrequencyFunction ff, GetAmplitudeFunction fa) :
    RealProcessBlock(name, size){
    getFrequencyFunction = ff;
    getAmplitudeFunction = fa;
    oscillatorType = t;
    index = 0;
  }

  void process(sample_t * samplesToProcess)
  {
    (void)samplesToProcess; //oscillators do not depend on input

    float freq = getFrequencyFunction();
    float freq_samples = freq/48000.0; //HARD CODED SAMPLE RATE
    period_samples = (uint32_t) (1.0/freq_samples);

    for(uint32_t i=0; i<num_samples; i++){
      outputBuffer[i] = getAmplitudeFunction() * oscillator_square(index, period_samples);
      index++;
      if(index > period_samples)
        index = 0;
    }
  }
};
