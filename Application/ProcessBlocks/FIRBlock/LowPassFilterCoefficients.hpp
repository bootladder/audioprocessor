#ifndef __LOWPASSFILTERCOEFFICIENTS_HPP__
#define __LOWPASSFILTERCOEFFICIENTS_HPP__
#include <stdint.h>
#include "constexpr_arm_sin_f32.hpp"
#define MAX_NUM_TAPS 512
#define SAMPLE_FREQUENCY 48000.0
#include "SamplingTypes.h"
constexpr int midi_value_to_cutoff(int value){
  return (value*100) + 1;
}

const sample_t PI = 3.141592653589793115997963468544185161590576171875;
struct LowPassFilterCoefficients {

  sample_t arr[128][MAX_NUM_TAPS]; //128 different MIDI values

  constexpr LowPassFilterCoefficients() : arr() {
    for(auto i=0; i<128; i++){

      sample_t * coeffs = arr[i];

      uint32_t cutoff = midi_value_to_cutoff(i);

      sample_t fc_normalized = (sample_t) (cutoff / SAMPLE_FREQUENCY);
      sample_t coefficient_sum = 0.0; //running total for normalization
      int numTaps = MAX_NUM_TAPS;

      for(int i=0; i<numTaps;i++)
        {
          int n = i - (numTaps/2);  //apply a time shift of numTaps/2
          if(n==0){  //sinc(1) = 1
            coeffs[i] = 1;
            coefficient_sum += coeffs[i];  //keep running sum
            continue;
          }
          sample_t x = 3.1415927*fc_normalized*(sample_t)n;
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
