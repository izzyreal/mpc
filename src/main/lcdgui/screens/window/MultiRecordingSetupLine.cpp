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
    track = std::clamp(i, -1, 63);
}

int MultiRecordingSetupLine::getTrack()
{
    return track;
}
