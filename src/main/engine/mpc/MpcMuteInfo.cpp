#include <engine/mpc/MpcMuteInfo.hpp>

using namespace ctoot::mpc;

MpcMuteInfo::MpcMuteInfo()
{
}

void MpcMuteInfo::setNote(int note)
{
    this->note = note;
}

void MpcMuteInfo::setDrum(int drum)
{
    this->drum = drum;
}

int MpcMuteInfo::getNote()
{
    return note;
}

int MpcMuteInfo::getDrum()
{
    return drum;
}

bool MpcMuteInfo::shouldMute(int otherNote, int otherDrum)
{
    if (note == otherNote && drum == otherDrum)
    {
        return true;
    }

    return false;
}