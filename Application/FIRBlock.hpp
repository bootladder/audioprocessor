#ifndef __FIRBLOCK_HPP__
#define __FIRBLOCK_HPP__

#include "ProcessBlockFunctions_FIRFilters.hpp"
#include "BSP_Audio_Buffer_Interface.h"
#include "stm32f769xx.h" //identifies device type for FPU_PRESENT
#include "arm_math.h"

#include "constexpr_arm_sin_f32.hpp"

#include "ProcessBlock.hpp"

extern "C"{
#include "MemoryLogger.h"
}

#define SAMPLE_FREQUENCY 48000.0
#define MAX_BLOCK_SIZE 2048
#define MAX_NUM_TAPS 512

struct LowPassFilterCoefficients {

  float arr[128][MAX_NUM_TAPS]; //128 different MIDI values

  constexpr LowPassFilterCoefficients() : arr() {
    for(auto i=0; i<128; i++){

      float * coeffs = arr[i];

      uint32_t cutoff = i * 100 + 1;

      float fc_normalized = (float) (cutoff / SAMPLE_FREQUENCY);
      float coefficient_sum = 0.0; //running total for normalization
      int numTaps = MAX_NUM_TAPS;

      for(int i=0; i<numTaps;i++)
        {
          int n = i - (numTaps/2);  //apply a time shift of numTaps/2
          if(n==0){  //sinc(1) = 1
            coeffs[i] = 1;
            coefficient_sum += coeffs[i];  //keep running sum
            continue;
          }
          float x = 3.1415927*fc_normalized*(float)n;
          coeffs[i] = fc_normalized * constexpr_arm_sin_f32(x) / x;

          coefficient_sum += coeffs[i];//keep running sum
        }

      //normalize coefficients to 1
      for(int i=0;i<numTaps;i++){
        coeffs[i] = coeffs[i] / coefficient_sum;
      }
    }
  }
};

static constexpr struct LowPassFilterCoefficients low_pass_filter_coefficients = LowPassFilterCoefficients();

class FIRBlock : public RealProcessBlock{
  float * filter_coefficients;
  float * firStateF32;
  arm_fir_instance_f32 S;


public:
  FIRBlock(uint32_t size, uint32_t cutoff) : RealProcessBlock(ProcessBlockFunctions_Gain2X, size){
    filter_coefficients = new float[MAX_NUM_TAPS];
    firStateF32 = new float[MAX_BLOCK_SIZE + MAX_NUM_TAPS - 1];
    assignCoefficientArray(cutoff);
    arm_fir_init_f32(&S, MAX_NUM_TAPS, (float*)low_pass_filter_coefficients.arr[40],
                     &firStateF32[0], MY_PROCESSING_BUFFER_SIZE_SAMPLES); //blocksize
    MemoryLogger_LogStringLn("FIR Coeffs calcualtd");
  }

  void process(sample_t * samplesToProcess){
    for(uint32_t i=0; i<num_samples; i++){
      inputBuffer[i] = samplesToProcess[i];
    }
    arm_fir_f32(&S, inputBuffer, outputBuffer, num_samples);
  }

  void assignCoefficientArray(uint8_t midivalue)
  {
    S.pCoeffs = (float * )low_pass_filter_coefficients.arr[midivalue];
  }

  //overriding setMIDIParameter to update coefficients here
  //now there's only 1 parameter but there could be other parameters in this block
  void setMIDIParameter(BlockParamIdentifier_t id, uint8_t value){
    (void)id;
    assignCoefficientArray(value);
  }

  //need to redefine midimessagereceived, i belive due to early binding
  void MIDIMessageReceived(MIDI_Message_t & msg){
    for(int i=0; i<midiAssignmentIndex; i++){
      if(midiAssignments[i].msg.type != msg.type)
        continue;
      if(midiAssignments[i].msg.id != msg.id)
        continue;

      setMIDIParameter(midiAssignments[i].paramId, msg.value);
    }
  }
};


#endif
