#include "MuteInfo.hpp"

using namespace mpc::engine;

MuteInfo::MuteInfo()
{
}

void MuteInfo::setNote(int note)
{
    this->note = note;
}

void MuteInfo::setDrum(int drum)
{
    this->drum = drum;
}

int MuteInfo::getNote()
{
    return note;
}

int MuteInfo::getDrum()
{
    return drum;
}

bool MuteInfo::shouldMute(int otherNote, int otherDrum)
{
    if (note == otherNote && drum == otherDrum)
    {
        return true;
    }

    return false;
}