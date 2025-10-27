#include "MuteInfo.hpp"

using namespace mpc::engine;

MuteInfo::MuteInfo() {}

void MuteInfo::setNote(int note)
{
    this->note = note;
}

void MuteInfo::setDrum(int drum)
{
    this->drum = drum;
}

int MuteInfo::getDrum() const
{
    return drum;
}

bool MuteInfo::shouldMute(int otherNote, int otherDrum) const
{
    if (note == otherNote && drum == otherDrum)
    {
        return true;
    }

    return false;
}
