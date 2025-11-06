#include "MultiRecordingSetupLine.hpp"

#include <algorithm>

using namespace mpc::lcdgui::screens::window;

void MultiRecordingSetupLine::setIn(const int inToUse)
{
    in = inToUse;
}

int MultiRecordingSetupLine::getIn() const
{
    return in;
}

void MultiRecordingSetupLine::setTrack(const int i)
{
    track = std::clamp(i, -1, 63);
}

int MultiRecordingSetupLine::getTrack() const
{
    return track;
}
