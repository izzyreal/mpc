#include "file/aps/ApsHeader.hpp"

#include "file/ByteUtil.hpp"
#include "MpcSpecs.hpp"

#include <cassert>

using namespace mpc::file::aps;

ApsHeader::ApsHeader(const std::vector<char> &loadBytes)
{
    valid = loadBytes[0] == APS_HEADER_MAGIC[0] &&
            loadBytes[1] == APS_HEADER_MAGIC[1];

    soundCount = mpc::file::ByteUtil::bytes2ushort({loadBytes[2], loadBytes[3]});

    assert(soundCount <= Mpc2000XlSpecs::MAX_SOUND_COUNT_IN_MEMORY);
}

ApsHeader::ApsHeader(const uint16_t soundCount)
{
    assert(soundCount <= Mpc2000XlSpecs::MAX_SOUND_COUNT_IN_MEMORY);

    saveBytes = std::vector<char>(4);

    const auto soundCountBytes = mpc::file::ByteUtil::ushort2bytes(soundCount);
    saveBytes[0] = 10;
    saveBytes[1] = 5;
    saveBytes[2] = soundCountBytes[0];
    saveBytes[3] = soundCountBytes[1];
}

bool ApsHeader::isValid() const
{
    return valid;
}

int ApsHeader::getSoundAmount() const
{
    return soundCount;
}

std::vector<char> ApsHeader::getBytes() const
{
    return saveBytes;
}
