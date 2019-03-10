#include <ui/sampler/window/ZoomGui.hpp>

using namespace mpc::ui::sampler::window;
using namespace std;

ZoomGui::ZoomGui()
{
}

/*
void ZoomGui::setZoomLevel(int i)
{
    if (i < 0 || i > 6) return;
    zoomLevel = i;
    setChanged();
    notifyObservers(string("zoomlevel"));
}

int ZoomGui::getZoomLevel()
{
    return zoomLevel;
}
*/

void ZoomGui::setSmplLngthFix(bool b)
{
    smplLngthFix = b;
    setChanged();
    notifyObservers(string("smpllngthfix"));
}

bool ZoomGui::isSmplLngthFix()
{
    return smplLngthFix;
}

void ZoomGui::setLoopLngthFix(bool b)
{
    loopLngthFix = b;
    setChanged();
    notifyObservers(string("looplngthfix"));
}

bool ZoomGui::isLoopLngthFix()
{
    return loopLngthFix;
}
