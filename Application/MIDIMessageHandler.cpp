extern "C"{
#include "MIDIMessageHandler.h"
#include "BSP_Fast_UART.h"
#include "MemoryLogger.h"
#include "tinyprintf.h"
#include "SerialLogger.h"
}

#include "MIDIMessageHandler.hpp"

#include "MIDI_Message.hpp"
#include "MIDIMap.hpp"


MIDIMap * midiMap = 0;

extern "C" void MIDIMessageHandler_Handle(MIDI_Message_t message)
{
  static char str[100];

  MIDIReceiver * block = midiMap->lookup(message);
  if(block == 0){
    int size = tfp_snprintf(str,100, "%d,%d,%d,NO MATCH\n",
          message.type, message.id, message.value );
    SerialLogger_Log(LOGTYPE_MIDI_MESSAGE_PROCESSED, (uint8_t *)str, size);
    return;
  }

  int size = tfp_snprintf(str,100, "%d,%d,%d,MATCH\n",
          message.type, message.id, message.value );
  SerialLogger_Log(LOGTYPE_MIDI_MESSAGE_PROCESSED, (uint8_t *)str, size);

  block->MIDIMessageReceived(message);
}

void MIDIMessageHandler_RegisterMIDIMap(MIDIMap & map)
{
  midiMap = &map;
}
