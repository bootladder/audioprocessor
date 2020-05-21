//
// Created by steve on 5/20/20.
//
#include "MIDIReceiver.hpp"

void MIDIReceiver::assignMIDIMessageToParameter(MIDI_Message_t msg, BlockParamIdentifier_t id){
  midiAssignments[midiAssignmentIndex].msg = msg;
  midiAssignments[midiAssignmentIndex].paramId = id;
  midiAssignmentIndex++;
}

void MIDIReceiver::MIDIMessageReceived(MIDI_Message_t &msg) {
  for (int i = 0; i < midiAssignmentIndex; i++) {
    if (midiAssignments[i].msg.type != msg.type)
      continue;
    if (midiAssignments[i].msg.id != msg.id)
      continue;

    setMIDIParameter(midiAssignments[i].paramId, msg.value);
  }
}

void MIDIReceiver::setMIDIParameter(BlockParamIdentifier_t id, int value)
{
  (void)id;(void)value;
}
