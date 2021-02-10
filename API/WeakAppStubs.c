//
// Created by steve on 5/21/20.
//
#include <stdint.h>
#include "MIDI_Message.hpp"

#define WEAK __attribute__((weak))
WEAK void AudioProcessor_Init(void){

}
WEAK int16_t * AudioProcessor_ProcessSampleBuffer(int16_t * sampleBuf, uint32_t num_samples)
{
  return sampleBuf;
}

WEAK void MIDIMessageHandler_Handle(MIDI_Message_t message){

}
