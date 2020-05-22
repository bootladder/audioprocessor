#ifndef __MIDI_MESSAGE_H__
#define __MIDI_MESSAGE_H__

#include <stdint.h>

#define MIDI_CONTROL_CHANGE 0xb0
#define MIDI_NOTE_ON 0x90
#define MIDI_NOTE_OFF 0x80

typedef struct {
  uint8_t type;
  uint8_t id;
  uint8_t value;
} MIDI_Message_t;

#endif
