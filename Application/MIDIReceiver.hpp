#ifndef __MIDIRECEIVER_HPP__
#define __MIDIRECEIVER_HPP__

#include "MIDI_Message.hpp"

typedef enum BlockParamIdentifier{
    PARAM_0 = 0,
    PARAM_1 = 1,
    PARAM_2 = 2,
    PARAM_3 = 3,
    PARAM_4 = 4,
    PARAM_5 = 5,
    PARAM_6 = 6,
    PARAM_7 = 7,
} BlockParamIdentifier_t;

class MIDIReceiver{

private:
    class MIDIAssignment{
    public:
        MIDI_Message_t msg;
        BlockParamIdentifier_t paramId;
    };

    MIDIAssignment midiAssignments[16];  //hard coded
    int midiAssignmentIndex = 0;

public:
    void assignMIDIMessageToParameter(MIDI_Message_t msg, BlockParamIdentifier_t id){
        midiAssignments[midiAssignmentIndex].msg = msg;
        midiAssignments[midiAssignmentIndex].paramId = id;
        midiAssignmentIndex++;
    }

    virtual void MIDIMessageReceived(MIDI_Message_t &msg){
        for(int i=0; i<midiAssignmentIndex; i++) {
            if (midiAssignments[i].msg.type != msg.type)
                continue;
            if (midiAssignments[i].msg.id != msg.id)
                continue;

            setMIDIParameter(midiAssignments[i].paramId, msg.value);
        }
    }


    virtual void setMIDIParameter(BlockParamIdentifier_t id, int value){
        (void)id;(void)value;
    }


};

#endif
 