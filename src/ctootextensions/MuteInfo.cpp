#include <ctootextensions/MuteInfo.hpp>

using namespace mpc::ctootextensions;

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

bool MuteInfo::muteMe(int note, int drum)
{
    if(this->note == note && this->drum == drum)
        return true;

    return false;
}