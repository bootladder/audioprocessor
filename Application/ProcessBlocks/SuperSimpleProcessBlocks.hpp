#include "ProcessBlock.hpp"
extern "C"{
#include "MemoryLogger.h"
#include "SerialLogger.h"
#include "tinyprintf.h"
}

class GainBlock : public ProcessBlock{
  float gain;
  float gainFactor;
public:
  GainBlock(const char * name, uint32_t size) :
    ProcessBlock(name, size){
    gain = 1.0;
    gainFactor = 8.0;
  }

  void setGainFactor(float factor){
    gainFactor = factor;
  }
  char * paramToString(BlockParamIdentifier_t id){
    (void)id;
    static char str[10];
    int gainFirstDigit = (int)gain;
    tfp_snprintf(str, 10, "%d", gainFirstDigit);
    float gainFirstDecimalfloat = (gain - gainFirstDigit) * 10.0;
    int gainFirstDecimal = int(gainFirstDecimalfloat);
    str[1] = '.';
    tfp_snprintf(&str[2], 7, "%d", gainFirstDecimal);
    str[9] = 0;
    return str;
  }

  void setMIDIParameter(BlockParamIdentifier_t id, int value){
    (void)id;

    gain = (float)value * gainFactor /128.0;

    static char str[100];
    int size = tfp_snprintf(str,100, "%s, Gain, %s\n", name, paramToString(id));
    SerialLogger_Log(LOGTYPE_BLOCKGRAPH_NODE_UPDATE, (uint8_t *)str, size);
  }

  void process(sample_t * samplesToProcess)
  {
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }
    for(uint32_t i=0; i<num_samples; i++){
      outputBuffer[i] = inputBuffer[i] * gain;
    }
  }
};

