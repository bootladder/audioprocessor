#ifndef __MIDIMAP_HPP__
#define __MIDIMAP_HPP__

#include "MIDI_Message.hpp"
#include "ProcessBlock.hpp"

class MIDIMapEntry
{
public:
  MIDI_Message_t msg;
  MIDIReceiver * midiReceiver;
};

class MIDIMap
{
  int index = 0;
  MIDIMapEntry table[64];

public:
  MIDIMap(){index = 0;}
  void addEntry(MIDI_Message_t msg, MIDIReceiver &midiReceiver){
    MIDIMapEntry e = {
      msg, &midiReceiver
    };
    table[index] = e;
    index++;
  }

    MIDIReceiver * lookup(MIDI_Message_t msg){
    for(int i=0; i<index; i++){
      //check equality with type and ID
      if(table[i].msg.type != msg.type)
        continue;
      if(table[i].msg.id != msg.id)
        continue;

      return table[i].midiReceiver;
    }
    return 0;
  }
};
#endif
