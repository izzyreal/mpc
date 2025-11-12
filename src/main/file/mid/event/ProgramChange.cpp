#include "file/mid/event/ProgramChange.hpp"

#include "file/mid/event/ChannelEvent.hpp"

using namespace mpc::file::mid::event;

ProgramChange::ProgramChange(int tick, int channel, int program)
    : ChannelEvent(tick, PROGRAM_CHANGE, channel, program, 0)
{
}

ProgramChange::ProgramChange(int tick, int delta, int channel, int program)
    : ChannelEvent(tick, delta, PROGRAM_CHANGE, channel, program, 0)
{
}

int ProgramChange::getProgramNumber() const
{
    return mValue1;
}

void ProgramChange::setProgramNumber(int p)
{
    mValue1 = p;
}
