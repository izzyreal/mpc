#include "file/aps/ApsDrumConfiguration.hpp"

#include "file/BitUtil.hpp"

using namespace mpc::file::aps;

ApsDrumConfiguration::ApsDrumConfiguration(const std::vector<char> &loadBytes)
{
    programIndex = loadBytes[5];
    receivePgmChangeEnabled = BitUtil::getBits(loadBytes[2])[7] == '1';
    receiveMidiVolumeEnabled = BitUtil::getBits(loadBytes[3])[7] == '1';
}

ApsDrumConfiguration::ApsDrumConfiguration(int program, bool recPgmChange,
                                           bool recMidiVolume)
{
    saveBytes = std::vector<char>(12);
    for (int i = 0; i < 9; i++)
    {
        saveBytes[i] = TEMPLATE[i];
    }

    for (int i = 0; i < 3; i++)
    {
        saveBytes[i + 9] = PADDING[i];
    }

    saveBytes[1] = program;
    saveBytes[2] = BitUtil::setBits(1, saveBytes[2], recPgmChange);
    saveBytes[3] = BitUtil::setBits(1, saveBytes[3], recMidiVolume);
    saveBytes[5] = program;
}

std::vector<char> ApsDrumConfiguration::TEMPLATE =
    std::vector<char>{0, 0, 1, 1, 127, 0, 1, 1, 127};
std::vector<char> ApsDrumConfiguration::PADDING = std::vector<char>{64, 0, 6};

int ApsDrumConfiguration::getProgramIndex() const
{
    return programIndex;
}

bool ApsDrumConfiguration::isReceivePgmChangeEnabled() const
{
    return receivePgmChangeEnabled;
}

bool ApsDrumConfiguration::isReceiveMidiVolumeEnabled() const
{
    return receiveMidiVolumeEnabled;
}

std::vector<char> ApsDrumConfiguration::getBytes()
{
    return saveBytes;
}
