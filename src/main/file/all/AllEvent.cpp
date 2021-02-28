#include "AllEvent.hpp"

#include "AllChannelPressureEvent.hpp"
#include "AllControlChangeEvent.hpp"
#include "AllNoteEvent.hpp"
#include "AllPitchBendEvent.hpp"
#include "AllPolyPressureEvent.hpp"
#include "AllProgramChangeEvent.hpp"
#include "AllSysExEvent.hpp"

#include <Util.hpp>

#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

#include <cmath>

#include <file/BitUtil.hpp>
#include <file/ByteUtil.hpp>

using namespace mpc::file::all;
using namespace mpc::sequencer;
using namespace moduru::file;
using namespace std;

vector<int> AllEvent::TICK_BYTE3_BIT_RANGE = vector<int>{ 0, 3 };

shared_ptr<Event> AllEvent::bytesToMpcEvent(const vector<char>& bytes)
{
    auto eventID = bytes[EVENT_ID_OFFSET];
    
    if (eventID < 0)
    {
        switch (bytes[EVENT_ID_OFFSET])
        {
            case POLY_PRESSURE_ID:
                return AllPolyPressureEvent::bytesToMpcEvent(bytes);
            case CONTROL_CHANGE_ID:
                return AllControlChangeEvent::bytesToMpcEvent(bytes);
            case PGM_CHANGE_ID:
                return AllPolyPressureEvent::bytesToMpcEvent(bytes);
            case CH_PRESSURE_ID:
                return AllChannelPressureEvent::bytesToMpcEvent(bytes);
            case PITCH_BEND_ID:
                return AllPitchBendEvent::bytesToMpcEvent(bytes);
            case SYS_EX_ID:
                return AllSysExEvent::bytesToMpcEvent(bytes);
        }
    }
    else
    {
        return AllNoteEvent::bytesToMpcEvent(bytes);
    }
}

vector<char> AllEvent::mpcEventToBytes(std::shared_ptr<mpc::sequencer::Event> event)
{
    auto note = dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event);
    auto polyPressure = dynamic_pointer_cast<mpc::sequencer::PolyPressureEvent>(event);
    auto controlChange = dynamic_pointer_cast<mpc::sequencer::ControlChangeEvent>(event);
    auto programChange = dynamic_pointer_cast<mpc::sequencer::ProgramChangeEvent>(event);
    auto channelPressure = dynamic_pointer_cast<mpc::sequencer::ChannelPressureEvent>(event);
    auto pitchBend = dynamic_pointer_cast<mpc::sequencer::PitchBendEvent>(event);
    auto sysEx = dynamic_pointer_cast<mpc::sequencer::SystemExclusiveEvent>(event);
    auto mixer = dynamic_pointer_cast<mpc::sequencer::MixerEvent>(event);
    
    if (note)
        return AllNoteEvent::mpcEventToBytes(note);
    else if (polyPressure)
        return AllPolyPressureEvent::mpcEventToBytes(polyPressure);
    else if (controlChange)
        return AllControlChangeEvent::mpcEventToBytes(controlChange);
    else if (programChange)
        return AllProgramChangeEvent::mpcEventToBytes(programChange);
    else if (channelPressure)
        return AllChannelPressureEvent::mpcEventToBytes(channelPressure);
    else if (pitchBend)
        return AllPitchBendEvent::mpcEventToBytes(pitchBend);
    else if (sysEx)
        return AllSysExEvent::mpcEventToBytes(sysEx);
    else if (mixer)
        return AllSysExEvent::mpcEventToBytes(mixer);
}

int AllEvent::readTick(const vector<char>& bytes)
{
    unsigned short s3 = BitUtil::removeUnusedBits(bytes[TICK_BYTE3_OFFSET], TICK_BYTE3_BIT_RANGE);
    
    int result = ByteUtil::bytes2ushort(vector<char>{ bytes[TICK_BYTE1_OFFSET], bytes[TICK_BYTE2_OFFSET] }) + (s3 * 65536);
    
    return result;
}

void AllEvent::writeTick(vector<char>& event, int tick)
{
    auto remainder = tick % 65536;
    auto ba = ByteUtil::ushort2bytes(remainder);
    event[TICK_BYTE1_OFFSET] = ba[0];
    event[TICK_BYTE2_OFFSET] = ba[1];
    auto s3 = static_cast<int16_t>(floor(tick / 65536.0));
    
    event[TICK_BYTE3_OFFSET] = BitUtil::stitchBytes(event[TICK_BYTE3_OFFSET], AllNoteEvent::DURATION_BYTE1_BIT_RANGE, static_cast<int8_t>(s3), TICK_BYTE3_BIT_RANGE);
    
    return event;
}
