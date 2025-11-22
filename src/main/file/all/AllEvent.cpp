#include "AllEvent.hpp"

#include "AllChannelPressureEvent.hpp"
#include "AllControlChangeEvent.hpp"
#include "AllNoteEvent.hpp"
#include "AllPitchBendEvent.hpp"
#include "AllPolyPressureEvent.hpp"
#include "AllProgramChangeEvent.hpp"
#include "AllSysExEvent.hpp"

#include <cassert>

#include "file/BitUtil.hpp"
#include "file/ByteUtil.hpp"

using namespace mpc::file::all;
using namespace mpc::sequencer;

std::vector<int> AllEvent::TICK_BYTE3_BIT_RANGE{0, 3};

EventState AllEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    if (const auto eventID = bytes[EVENT_ID_OFFSET]; eventID < 0)
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
            default:;
        }
    }

    return AllNoteOnEvent::bytesToMpcEvent(bytes);
}

std::vector<char> AllEvent::mpcEventToBytes(const EventState &e)
{
    assert(e.type != sequencer::EventType::NoteOff);

    if (e.type == EventType::NoteOn)
    {
        return AllNoteOnEvent::mpcEventToBytes(e);
    }
    if (e.type == EventType::PolyPressure)
    {
        return AllPolyPressureEvent::mpcEventToBytes(e);
    }
    if (e.type == EventType::ControlChange)
    {
        return AllControlChangeEvent::mpcEventToBytes(e);
    }
    if (e.type == EventType::ProgramChange)
    {
        return AllProgramChangeEvent::mpcEventToBytes(e);
    }
    if (e.type == EventType::ChannelPressure)
    {
        return AllChannelPressureEvent::mpcEventToBytes(e);
    }
    if (e.type == EventType::PitchBend)
    {
        return AllPitchBendEvent::mpcEventToBytes(e);
    }
    if (e.type == EventType::SystemExclusive || e.type == EventType::Mixer)
    {
        return AllSysExEvent::mpcEventToBytes(e);
    }

    return {};
}

int AllEvent::readTick(const std::vector<char> &bytes)
{
    unsigned short s3 = BitUtil::removeUnusedBits(bytes[TICK_BYTE3_OFFSET],
                                                  TICK_BYTE3_BIT_RANGE);

    int result = ByteUtil::bytes2ushort(std::vector{bytes[TICK_BYTE1_OFFSET],
                                                    bytes[TICK_BYTE2_OFFSET]}) +
                 s3 * 65536;

    return result;
}

void AllEvent::writeTick(std::vector<char> &event, int tick)
{
    auto remainder = tick % 65536;
    auto ba = ByteUtil::ushort2bytes(remainder);
    event[TICK_BYTE1_OFFSET] = ba[0];
    event[TICK_BYTE2_OFFSET] = ba[1];
    auto s3 = static_cast<int16_t>(floor(tick / 65536.0));

    event[TICK_BYTE3_OFFSET] = BitUtil::stitchBytes(
        static_cast<unsigned char>(event[TICK_BYTE3_OFFSET]),
        AllNoteOnEvent::DURATION_BYTE1_BIT_RANGE,
        static_cast<unsigned char>(s3), TICK_BYTE3_BIT_RANGE);
}
