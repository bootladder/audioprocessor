#ifndef __LOWPASSFILTERCOEFFICIENTS_HPP__
#define __LOWPASSFILTERCOEFFICIENTS_HPP__
#include <stdint.h>
#include "constexpr_arm_sin_f32.hpp"
#define MAX_NUM_TAPS 512
#define SAMPLE_FREQUENCY 48000.0

constexpr int midi_value_to_cutoff(int value){
  return (value*100) + 1;
}


struct LowPassFilterCoefficients {

  float arr[128][MAX_NUM_TAPS]; //128 different MIDI values

  constexpr LowPassFilterCoefficients() : arr() {
    for(auto i=0; i<128; i++){

      float * coeffs = arr[i];

      uint32_t cutoff = midi_value_to_cutoff(i);

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

#endif
