extern "C"{  
#include "MIDIMessageHandler.h"
#include "tinyprintf.h"
#include "SerialLogger.h"
}

#include "MIDI_Message.hpp"
#include "MIDIMap.hpp"

// There is only 1 MIDIMessageHandler so this is a global
static char str[100];
static MIDIMap * midiMap = 0;

static void log_midi_match_message(MIDI_Message_t message);
static void log_midi_nomatch_message(MIDI_Message_t message);


// Used by Application after it populates the MIDIMap
void MIDIMessageHandler_RegisterMIDIMap(MIDIMap & map)
{
  midiMap = &map;
}

// Called by the task that receives MIDI Messages
extern "C" void MIDIMessageHandler_Handle(MIDI_Message_t message)
{
  MIDIReceiver * block = midiMap->lookup(message);

  if(block == 0){
    log_midi_match_message(message);
    return;
  }

  log_midi_nomatch_message(message);
  block->MIDIMessageReceived(message);
}

static void log_midi_nomatch_message(MIDI_Message_t message) {
  int size = tfp_snprintf(str,100, "%d,%d,%d,MATCH\n",
                          message.type, message.id, message.value );
  SerialLogger_Log(LOGTYPE_MIDI_MESSAGE_PROCESSED, (uint8_t *)str, (uint32_t)size);
}

static void log_midi_match_message(MIDI_Message_t message) {
  int size = tfp_snprintf(str,100, "%d,%d,%d,NO MATCH\n",
                          message.type, message.id, message.value );
  SerialLogger_Log(LOGTYPE_MIDI_MESSAGE_PROCESSED, (uint8_t *)str, (uint32_t)size);
}

