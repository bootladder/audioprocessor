extern "C"{
#include "MIDIMessageHandler.h"
}

#include "MIDIMessageHandler.hpp"

#include "MIDI_Message.h"
#include "MIDIMap.hpp"

MIDIMap midiMap;

extern "C" void MIDIMessageHandler_Handle(MIDI_Message_t message)
{
  ProcessBlock * block = midiMap.lookup(message);
  block->MIDIMessageReceived(message);
}

void MIDIMessageHandler_RegisterMIDIMap(MIDIMap & map)
{
  midiMap = map;
}
