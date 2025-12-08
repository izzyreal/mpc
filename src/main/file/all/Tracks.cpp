#include "file/all/Tracks.hpp"

#include "file/all/AllParser.hpp"
#include "file/all/AllSequence.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"

#include "file/ByteUtil.hpp"
#include "StrUtil.hpp"
#include "Util.hpp"

using namespace mpc::file::all;

Tracks::Tracks(const std::vector<char> &loadBytes)
{
    for (int i = 0; i < 64; i++)
    {
        devices[i] = loadBytes[DEVICES_OFFSET + i];
        busses[i] = loadBytes[BUSSES_OFFSET + i];
        pgms[i] = loadBytes[PGMS_OFFSET + i];
        veloRatios[i] = loadBytes[VELO_RATIOS_OFFSET + i];
        const auto offset = TRACK_NAMES_OFFSET + i * AllParser::NAME_LENGTH;
        std::string name;

        for (const char c : Util::vecCopyOfRange(
                 loadBytes, offset, offset + AllParser::NAME_LENGTH))
        {
            if (c == 0x00)
            {
                break;
            }
            name.push_back(c);
        }

        names[i] = name;

        const auto statusByte = loadBytes[STATUS_OFFSET + i];

        usednesses[i] = (statusByte >> 0) & 1;
        ons[i] = (statusByte >> 1) & 1;
        transmitProgramChangesEnableds[i] = (statusByte >> 2) & 1;
    }
}

Tracks::Tracks(sequencer::Sequence *seq)
{
    saveBytes = std::vector<char>(AllSequence::TRACKS_LENGTH);
    for (int i = 0; i < 64; i++)
    {
        const auto t = seq->getTrack(i);

        for (auto j = 0; j < AllParser::NAME_LENGTH; j++)
        {
            const auto offset = TRACK_NAMES_OFFSET + i * AllParser::NAME_LENGTH;
            auto name = StrUtil::padRight(t->getActualName(), " ",
                                          AllParser::NAME_LENGTH);
            saveBytes[offset + j] = name[j];
        }

        saveBytes[DEVICES_OFFSET + i] = t->getDeviceIndex();
        saveBytes[BUSSES_OFFSET + i] = busTypeToIndex(t->getBusType());
        saveBytes[PGMS_OFFSET + i] = t->getProgramChange();
        saveBytes[VELO_RATIOS_OFFSET + i] = t->getVelocityRatio();

        uint8_t b = 0;
        if (t->isUsed())
        {
            b |= 1u << 0;
        }
        if (t->isOn())
        {
            b |= 1u << 1;
        }
        if (t->isTransmitProgramChangesEnabled())
        {
            b |= 1u << 2;
        }
        saveBytes[STATUS_OFFSET + i] = b;
    }

    for (int i = 0; i < PADDING1.size(); i++)
    {
        saveBytes[PADDING1_OFFSET + i] = PADDING1[i];
    }

    const auto lastTick = seq->getLastTick();
    const auto remainder = lastTick % 65535;
    const auto large = static_cast<int>(floor(lastTick / 65536.0));
    const auto lastTickBytes = ByteUtil::ushort2bytes(remainder);
    saveBytes[LAST_TICK_BYTE1_OFFSET] = lastTickBytes[0];
    saveBytes[LAST_TICK_BYTE2_OFFSET] = lastTickBytes[1];
    saveBytes[LAST_TICK_BYTE3_OFFSET] = large;

    const auto unknown32BitIntBytes1 = ByteUtil::uint2bytes(10000000);
    const auto unknown32BitIntBytes2 = ByteUtil::uint2bytes(
        static_cast<int>(seq->getLastTick() * 5208.333333333333));

    for (int j = 0; j < 4; j++)
    {
        const int offset = UNKNOWN32_BIT_INT_OFFSET + j;
        saveBytes[offset] = unknown32BitIntBytes1[j];
    }

    for (int j = 0; j < 4; j++)
    {
        saveBytes[UNKNOWN32_BIT_INT_OFFSET + j + 4] = unknown32BitIntBytes2[j];
    }
}

std::vector<char> Tracks::PADDING1 = std::vector<char>{
    static_cast<char>(232), 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    static_cast<char>(232), 3};

int Tracks::getDevice(const int i) const
{
    return devices[i];
}

int Tracks::getBus(const int i) const
{
    return busses[i];
}

int Tracks::getVelo(const int i) const
{
    return veloRatios[i];
}

int Tracks::getPgm(const int i) const
{
    return pgms[i];
}

std::string Tracks::getName(const int i)
{
    return names[i];
}

bool Tracks::isUsed(const int i) const
{
    return usednesses[i];
}

bool Tracks::isOn(const int i) const
{
    return ons[i];
}

bool Tracks::isTransmitProgramChangesEnabled(const int i)
{
    return transmitProgramChangesEnableds[i];
}

std::vector<char> &Tracks::getBytes()
{
    return saveBytes;
}
