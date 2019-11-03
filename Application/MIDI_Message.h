#ifndef __MIDI_MESSAGE_H__
#define __MIDI_MESSAGE_H__

#include <stdint.h>

typedef struct {
  uint8_t type;
  uint8_t id;
  uint8_t value;
} MIDI_Message_t;

#endif
