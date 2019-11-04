extern "C"{
#include "MIDIMessageHandler.h"
#include "BSP_Fast_UART.h"
#include "My_Logger.h"
}

#include "MIDIMessageHandler.hpp"

#include "MIDI_Message.h"
#include "MIDIMap.hpp"

MIDIMap * midiMap = 0;

extern "C" void MIDIMessageHandler_Handle(MIDI_Message_t message)
{
  ProcessBlock * block = midiMap->lookup(message);
  if(block == 0)
    return;

  block->MIDIMessageReceived(message);
  My_Logger_LogStringLn("hurr");
  //SerialLogger_PrintLiteralString("Hurr Durr I'm a string literal\n");
}

void MIDIMessageHandler_RegisterMIDIMap(MIDIMap & map)
{
  midiMap = &map;
}
