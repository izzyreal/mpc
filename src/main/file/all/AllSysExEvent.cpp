#include "AllSysExEvent.hpp"

#include "AllEvent.hpp"
#include "AllSequence.hpp"
#include "Util.hpp"

using namespace mpc::file::all;
using namespace mpc::performance;

std::vector<char> AllSysExEvent::MIXER_SIGNATURE = {static_cast<char>(240), 71, 0, 68, 69};

Event
AllSysExEvent::bytesToMpcEvent(const std::vector<char> &bytes)
{
    const int byteCount = bytes[BYTE_COUNT_OFFSET];

    std::vector<char> sysexLoadData(byteCount);

    Event e;

    for (int i = 0; i < byteCount; i++)
    {
        sysexLoadData[i] = bytes[DATA_OFFSET + i];
    }

    if (Util::vecEquals(Util::vecCopyOfRange(
                            sysexLoadData, MIXER_SIGNATURE_OFFSET,
                            MIXER_SIGNATURE_OFFSET + MIXER_SIGNATURE.size()),
                        MIXER_SIGNATURE))
    {
        e.type = EventType::Mixer;

        auto paramCandidate = sysexLoadData[MIXER_PARAMETER_OFFSET] - 1;

        if (paramCandidate == 4)
        {
            paramCandidate = 3;
        }

        e.mixerParameter = paramCandidate;
        e.mixerPad = sysexLoadData[MIXER_PAD_OFFSET];
        e.mixerValue = sysexLoadData[MIXER_VALUE_OFFSET];
        e.tick = AllEvent::readTick(bytes);
    }
    else
    {
        e.type = EventType::SystemExclusive;
        // e.sysexData = sysexLoadData;
        e.tick =AllEvent::readTick(bytes);
    }

    e.trackIndex = TrackIndex(bytes[AllEvent::TRACK_OFFSET]);

    return e;
}

std::vector<char>
AllSysExEvent::mpcEventToBytes(const Event &e)
{
    std::vector<char> bytes;

    if (e.type == EventType::Mixer)
    {
        bytes = std::vector<char>(32);

        AllEvent::writeTick(bytes, e.tick);
        bytes[AllEvent::TRACK_OFFSET] = e.trackIndex;
        bytes[CHUNK_HEADER_ID_OFFSET] = HEADER_ID;
        bytes[BYTE_COUNT_OFFSET] = 9;
        bytes[DATA_HEADER_ID_OFFSET] = HEADER_ID;
        for (int i = 0; i < MIXER_SIGNATURE.size(); i++)
        {
            bytes[DATA_OFFSET + i] = MIXER_SIGNATURE[i];
        }

        bytes[DATA_OFFSET + MIXER_PAD_OFFSET] =
            e.mixerPad;
        auto paramCandidate = e.mixerParameter;

        if (paramCandidate == 3)
        {
            paramCandidate = 4;
        }

        paramCandidate++;
        bytes[DATA_OFFSET + MIXER_PARAMETER_OFFSET] = paramCandidate;
        bytes[DATA_OFFSET + MIXER_VALUE_OFFSET] = e.mixerValue;
        bytes[DATA_OFFSET + MIXER_VALUE_OFFSET + 1] = DATA_TERMINATOR_ID;
        bytes[MIX_TERMINATOR_ID_OFFSET] = CHUNK_TERMINATOR_ID;
    }
    else if (e.type == EventType::SystemExclusive)
    {
        AllEvent::writeTick(bytes, e.tick);
        constexpr int dataSize = 0;
        constexpr int dataSegments = static_cast<int>(dataSize / 8.0);
        bytes = std::vector<char>((dataSegments + 2) *
                                  AllSequence::EVENT_SEG_LENGTH);
        bytes[AllEvent::TRACK_OFFSET] = e.trackIndex;
        bytes[AllEvent::TRACK_OFFSET +
              (dataSegments + 1) * AllSequence::EVENT_SEG_LENGTH] =
            e.trackIndex;
        bytes[CHUNK_HEADER_ID_OFFSET] = HEADER_ID;
        bytes[BYTE_COUNT_OFFSET] = static_cast<char>(dataSize);

        for (int i = 0; i < dataSize; i++)
        {
            // bytes[DATA_OFFSET + i] = sysExEvent->getBytes()[i];
        }

        bytes[static_cast<int>(bytes.size()) - 4] = CHUNK_TERMINATOR_ID;
    }

    return bytes;
}
