#ifndef __MIDIMAP_HPP__
#define __MIDIMAP_HPP__

#include "MIDI_Message.h"
#include "ProcessBlock.hpp"

class MIDIMapEntry
{
public:
  MIDI_Message_t msg;
  ProcessBlock * block;
};

class MIDIMap
{
  int index = 0;
  MIDIMapEntry table[64];

public:
  void addEntry(MIDI_Message_t &msg, ProcessBlock &block){
    MIDIMapEntry e = {
      msg, &block
    };
    table[index] = e;
    index++;
  }

  ProcessBlock * lookup(MIDI_Message_t & msg){
    for(int i=0; i<index; i++){
      //check equality with type and ID
      if(table[i].msg.type != msg.type)
        continue;
      if(table[i].msg.id != msg.id)
        continue;

      return table[i].block;
    }
    return 0;
  }
};
#endif
