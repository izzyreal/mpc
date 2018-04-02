#include <midi/event/ProgramChange.hpp>

#include <midi/event/ChannelEvent.hpp>

using namespace mpc::midi::event;

ProgramChange::ProgramChange(int tick, int channel, int program) : ChannelEvent(tick, ChannelEvent::PROGRAM_CHANGE, channel, program, 0)
{
}

ProgramChange::ProgramChange(int tick, int delta, int channel, int program) : ChannelEvent(tick, delta, ChannelEvent::PROGRAM_CHANGE, channel, program, 0)
{
}

int ProgramChange::getProgramNumber()
{
    return mValue1;
}

void ProgramChange::setProgramNumber(int p)
{
    mValue1 = p;
}
