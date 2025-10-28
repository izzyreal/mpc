#include "MultiRecordingSetupLine.hpp"

#include <algorithm>

using namespace mpc::lcdgui::screens::window;

void MultiRecordingSetupLine::setIn(int inToUse)
{
    in = inToUse;
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
