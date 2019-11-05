#ifndef __FIRBLOCK_HPP__
#define __FIRBLOCK_HPP__


#include "ProcessBlockFunctions_FIRFilters.hpp"
#include "BSP_Audio_Buffer_Interface.h"
#include "stm32f769xx.h" //identifies device type for FPU_PRESENT
#include "arm_math.h"

#include "ProcessBlock.hpp"

extern "C"{
#include "SerialLogger.h"
#include "MemoryLogger.h"
}

#define SAMPLE_FREQUENCY 48000.0
#define MAX_BLOCK_SIZE 2048
#define MAX_NUM_TAPS 2048


class FIRBlock : public RealProcessBlock{
  float * filter_coefficients;
  float * firStateF32;
  arm_fir_instance_f32 S;

public:
  FIRBlock(uint32_t size, uint32_t cutoff) : RealProcessBlock(ProcessBlockFunctions_Gain2X, size){
    filter_coefficients = new float[MAX_NUM_TAPS];
    firStateF32 = new float[MAX_BLOCK_SIZE + MAX_NUM_TAPS - 1];
    calculateCoefficients(cutoff);
    MemoryLogger_LogStringLn("FIR Coeffs calcualtd");
  }

  void process(sample_t * samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }
    arm_fir_f32(&S, inputBuffer, outputBuffer, num_samples);
  }

  void calculateCoefficients(uint32_t cutoff)
  {
    float fc_normalized = (float) (cutoff / SAMPLE_FREQUENCY);
    float coefficient_sum = 0.0;
    int numTaps = 512;

    for(int i=0; i<numTaps;i++)
      {
        int n = i - (numTaps/2);  //apply a time shift of numTaps/2
        if(n==0){  //sinc(1) = 1
          filter_coefficients[i] = 1;
          coefficient_sum += filter_coefficients[i];
          continue;
        }
        float x = 3.1415927*fc_normalized*(float)n;
        filter_coefficients[i] = fc_normalized * arm_sin_f32(x) / x;

        coefficient_sum += filter_coefficients[i];
      }

    //normalize coefficients to 1
    for(int i=0;i<numTaps;i++){
      filter_coefficients[i] = filter_coefficients[i] / coefficient_sum;
    }

    arm_fir_init_f32(&S, numTaps, filter_coefficients, &firStateF32[0], MY_PROCESSING_BUFFER_SIZE_SAMPLES); //blocksize
  }

  //overriding setParam to update coefficients here
  void setParam(BlockParamIdentifier_t id, int value){

    uint32_t new_cutoff = value * 10000 / 128;
    calculateCoefficients(new_cutoff);
  }

  //need to redefine midimessagereceived, i belive due to early binding
  void MIDIMessageReceived(MIDI_Message_t & msg){
    for(int i=0; i<midiAssignmentIndex; i++){
      if(midiAssignments[i].msg.type != msg.type)
        continue;
      if(midiAssignments[i].msg.id != msg.id)
        continue;

      setParam(midiAssignments[i].paramId, msg.value);
    }
  }


};
#endif
