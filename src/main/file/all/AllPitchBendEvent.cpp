#include "file/all/AllPitchBendEvent.hpp"

#include "file/all/AllEvent.hpp"

#include "file/ByteUtil.hpp"

using namespace mpc::file::all;
using namespace mpc::performance;

Event
AllPitchBendEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    Event e;
    e.type = EventType::PitchBend;
    e.tick = AllEvent::readTick(bytes);
    e.trackIndex = TrackIndex(bytes[AllEvent::TRACK_OFFSET]);

    auto candidate = ByteUtil::bytes2ushort(std::vector{
                         bytes[AMOUNT_OFFSET], bytes[AMOUNT_OFFSET + 1]}) -
                     16384;

    if (candidate < -8192)
    {
        candidate += 8192;
    }

    e.amount = candidate;

    return e;
}

std::vector<char>
AllPitchBendEvent::mpcEventToBytes(const Event &e)
{
    std::vector<char> bytes(8);
    bytes[AllEvent::EVENT_ID_OFFSET] = AllEvent::PITCH_BEND_ID;
    AllEvent::writeTick(bytes, e.tick);
    bytes[AllEvent::TRACK_OFFSET] = e.trackIndex;

    auto candidate = e.amount + 16384;

    if (e.amount < 0)
    {
        candidate = e.amount + 8192;
    }

    const auto amountBytes = ByteUtil::ushort2bytes(candidate);

    bytes[AMOUNT_OFFSET] = amountBytes[0];
    bytes[AMOUNT_OFFSET + 1] = amountBytes[1];

    return bytes;
}
