#include "MultiRecordingSetupLine.hpp"

using namespace mpc::lcdgui::screens::window;

void MultiRecordingSetupLine::setIn(int in)
{
    this->in = in;
}

int MultiRecordingSetupLine::getIn()
{
    return in;
}

void MultiRecordingSetupLine::setTrack(int i)
{
    if (i < -1 || i > 63)
        return;

    track = i;
}

int MultiRecordingSetupLine::getTrack()
{
    return track;
}

void MultiRecordingSetupLine::setOut(int i)
{
    if (i < 0 || i > 32)
        return;
    out = i;
}

int MultiRecordingSetupLine::getOut()
{
    return out;
}
