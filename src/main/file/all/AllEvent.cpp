#include "AllEvent.hpp"

#include "AllChannelPressureEvent.hpp"
#include "AllControlChangeEvent.hpp"
#include "AllNoteEvent.hpp"
#include "AllPitchBendEvent.hpp"
#include "AllPolyPressureEvent.hpp"
#include "AllProgramChangeEvent.hpp"
#include "AllSysExEvent.hpp"

#include "sequencer/ChannelPressureEvent.hpp"
#include "sequencer/ControlChangeEvent.hpp"
#include "sequencer/Event.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/PitchBendEvent.hpp"
#include "sequencer/PolyPressureEvent.hpp"
#include "sequencer/ProgramChangeEvent.hpp"
#include "sequencer/SystemExclusiveEvent.hpp"

#include <cassert>
#include <cmath>

#include "file/BitUtil.hpp"
#include "file/ByteUtil.hpp"

using namespace mpc::file::all;
using namespace mpc::sequencer;

std::vector<int> AllEvent::TICK_BYTE3_BIT_RANGE{ 0, 3 };

std::shared_ptr<Event> AllEvent::bytesToMpcEvent(const std::vector<char>& bytes)
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

    return AllNoteOnEvent::bytesToMpcEvent(bytes);
}

std::vector<char> AllEvent::mpcEventToBytes(const std::shared_ptr<Event>& event)
{
    assert(!std::dynamic_pointer_cast<NoteOffEvent>(event));
    
    if (const auto noteOn = std::dynamic_pointer_cast<NoteOnEvent>(event); noteOn)
    {
        return AllNoteOnEvent::mpcEventToBytes(noteOn);
    }
    else if (const auto polyPressure = std::dynamic_pointer_cast<PolyPressureEvent>(event); polyPressure)
    {
        return AllPolyPressureEvent::mpcEventToBytes(polyPressure);
    }
    else if (const auto controlChange = std::dynamic_pointer_cast<ControlChangeEvent>(event); controlChange)
    {
        return AllControlChangeEvent::mpcEventToBytes(controlChange);
    }
    else if (const auto programChange = std::dynamic_pointer_cast<ProgramChangeEvent>(event); programChange)
    {
        return AllProgramChangeEvent::mpcEventToBytes(programChange);
    }
    else if (const auto channelPressure = std::dynamic_pointer_cast<ChannelPressureEvent>(event); channelPressure)
    {
        return AllChannelPressureEvent::mpcEventToBytes(channelPressure);
    }
    else if (const auto pitchBend = std::dynamic_pointer_cast<PitchBendEvent>(event); pitchBend)
    {
        return AllPitchBendEvent::mpcEventToBytes(pitchBend);
    }
    else if (const auto sysEx = std::dynamic_pointer_cast<SystemExclusiveEvent>(event); sysEx)
    {
        return AllSysExEvent::mpcEventToBytes(sysEx);
    }
    else if (const auto mixer = std::dynamic_pointer_cast<MixerEvent>(event); mixer)
    {
        return AllSysExEvent::mpcEventToBytes(mixer);
    }

    return {};
}

int AllEvent::readTick(const std::vector<char>& bytes)
{
    unsigned short s3 = BitUtil::removeUnusedBits(bytes[TICK_BYTE3_OFFSET], TICK_BYTE3_BIT_RANGE);
    
    int result = ByteUtil::bytes2ushort(std::vector<char>{ bytes[TICK_BYTE1_OFFSET], bytes[TICK_BYTE2_OFFSET] }) + (s3 * 65536);
    
    return result;
}

void AllEvent::writeTick(std::vector<char>& event, int tick)
{
    auto remainder = tick % 65536;
    auto ba = ByteUtil::ushort2bytes(remainder);
    event[TICK_BYTE1_OFFSET] = ba[0];
    event[TICK_BYTE2_OFFSET] = ba[1];
    auto s3 = static_cast<int16_t>(floor(tick / 65536.0));
    
    event[TICK_BYTE3_OFFSET] = BitUtil::stitchBytes(static_cast<unsigned char>(event[TICK_BYTE3_OFFSET]), AllNoteOnEvent::DURATION_BYTE1_BIT_RANGE, static_cast<unsigned char>(s3), TICK_BYTE3_BIT_RANGE);
}
