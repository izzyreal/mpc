#include "midi/event/meta/FrameRate.hpp"

#include <stdexcept>

#ifdef __linux__
#include <stdexcept>
#endif // __linux__

using namespace mpc::midi::event::meta;

FrameRate::FrameRate(std::string name, int ordinal, int v)
{
    this->name = name;
    this->ordinal = ordinal;
    value = v;
}

FrameRate *FrameRate::FRAME_RATE_24 = new ::FrameRate("FRAME_RATE_24", 0, 0);
FrameRate *FrameRate::FRAME_RATE_25 = new ::FrameRate("FRAME_RATE_25", 1, 1);
FrameRate *FrameRate::FRAME_RATE_30_DROP = new ::FrameRate("FRAME_RATE_30_DROP", 2, 2);
FrameRate *FrameRate::FRAME_RATE_30 = new ::FrameRate("FRAME_RATE_30", 3, 3);

FrameRate *FrameRate::fromInt(int val)
{
    switch (val)
    {
    case 0:
        return FRAME_RATE_24;
    case 1:
        return FRAME_RATE_25;
    case 2:
        return FRAME_RATE_30_DROP;
    case 3:
        return FRAME_RATE_30;
    }
    return nullptr;
}

FrameRate *FrameRate::valueOf(std::string a0)
{
    if (FRAME_RATE_24->name == a0)
    {
        return FRAME_RATE_24;
    }
    if (FRAME_RATE_25->name == a0)
    {
        return FRAME_RATE_25;
    }
    if (FRAME_RATE_30->name == a0)
    {
        return FRAME_RATE_30;
    }
    if (FRAME_RATE_30_DROP->name == a0)
    {
        return FRAME_RATE_30_DROP;
    }
    throw std::invalid_argument("Illegal argument");
}

std::vector<FrameRate *> FrameRate::values()
{
    return std::vector<FrameRate *>{
        FRAME_RATE_24,
        FRAME_RATE_25,
        FRAME_RATE_30,
        FRAME_RATE_30_DROP,
    };
}
