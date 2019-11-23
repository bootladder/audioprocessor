extern "C"{
#include "MIDIMessageHandler.h"
#include "BSP_Fast_UART.h"
#include "MemoryLogger.h"
#include "tinyprintf.h"
#include "SerialLogger.h"
}

#include "MIDIMessageHandler.hpp"

#include "MIDI_Message.h"
#include "MIDIMap.hpp"


MIDIMap * midiMap = 0;

extern "C" void MIDIMessageHandler_Handle(MIDI_Message_t message)
{
  ProcessBlock * block = midiMap->lookup(message);
  if(block == 0){
    SerialLogger_LogLiteralString(LOGTYPE_MIDI_MESSAGE_PROCESSED, "MIDI IN:  NO MATCH\n");
    return;
  }

  static char str[100];
  int size = tfp_snprintf(str,100, "MIDI MATCH: Block Addr: %p \n", block);
  SerialLogger_Log(LOGTYPE_MIDI_MESSAGE_PROCESSED, (uint8_t *)str, size);

  block->MIDIMessageReceived(message);
}

void MIDIMessageHandler_RegisterMIDIMap(MIDIMap & map)
{
  midiMap = &map;
}
