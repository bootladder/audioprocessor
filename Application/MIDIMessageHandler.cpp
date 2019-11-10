extern "C"{
#include "MIDIMessageHandler.h"
#include "BSP_Fast_UART.h"
#include "MemoryLogger.h"
}

#include "MIDIMessageHandler.hpp"

#include "MIDI_Message.h"
#include "MIDIMap.hpp"

MIDIMap * midiMap = 0;

extern "C" void MIDIMessageHandler_Handle(MIDI_Message_t message)
{
  SerialLogger_LogLiteralString(LOGTYPE_EVENT, "MIDI IN: ");

  ProcessBlock * block = midiMap->lookup(message);
  if(block == 0)
    return;

  SerialLogger_LogLiteralString(LOGTYPE_EVENT, "MATCH\n");

  block->MIDIMessageReceived(message);
}

void MIDIMessageHandler_RegisterMIDIMap(MIDIMap & map)
{
  midiMap = &map;
}
