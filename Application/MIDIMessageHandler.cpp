extern "C"{
#include "MIDIMessageHandler.h"
}

#include "MIDI_Message.h"

extern "C" void MIDIMessageHandler_Handle(MIDI_Message_t message)
{
  (void)message;
}
