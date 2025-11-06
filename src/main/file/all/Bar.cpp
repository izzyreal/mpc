#include "file/all/Bar.hpp"

#include "file/ByteUtil.hpp"

using namespace mpc::file::all;

Bar::Bar(const std::vector<char> &bytes, Bar *previousBar)
{
    ticksPerBeat = bytes[0] & 255;
    auto intVal = ByteUtil::bytes2ushort({bytes[1], bytes[2]});
    lastTick = ((bytes[3] & 255) * 65536) + intVal;
    barLength = lastTick - (previousBar == nullptr ? 0 : previousBar->lastTick);
}

Bar::Bar(int ticksPerBeat, int lastTick)
{
    saveBytes = std::vector<char>(4);
    saveBytes[0] = static_cast<int8_t>(ticksPerBeat);
    auto intVal = lastTick % 65536;
    auto bytePair = ByteUtil::ushort2bytes(intVal);
    saveBytes[1] = bytePair[0];
    saveBytes[2] = bytePair[1];
    saveBytes[3] = static_cast<int8_t>(((lastTick - intVal) / 65536));
}

int Bar::getTicksPerBeat() const
{
    return ticksPerBeat;
}

int Bar::getDenominator() const
{
    auto result = 0;
    switch (ticksPerBeat)
    {
        case 96:
            result = 4;
            break;
        case 48:
            result = 8;
            break;
        case 24:
            result = 16;
            break;
        case 12:
            result = 32;
            break;
    }

    return result;
}

int Bar::getNumerator() const
{
    return barLength / ticksPerBeat;
}

int Bar::getLastTick() const
{
    return lastTick;
}

std::vector<char> &Bar::getBytes()
{
    return saveBytes;
}
