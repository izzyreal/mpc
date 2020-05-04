#include <ui/sequencer/window/EraseGui.hpp>

using namespace mpc::ui::sequencer::window;
using namespace std;

EraseGui::EraseGui()
{
}

void EraseGui::setTrack(int i)
{
    if (i < -1 || i > 63)
    {
        return;
    }

    track = i;
    setChanged();
    notifyObservers(string("track"));
}

void EraseGui::setErase(int i)
{
    if (i < 0 || i > 2) {
        return;
    }

    erase = i;
    setChanged();
    notifyObservers(string("erase"));
}

void EraseGui::setType(int i)
{
    if (i < 0 || i > 6) {
        return;
    }

    type = i;
    setChanged();
    notifyObservers(string("type"));
}

int EraseGui::getTrack()
{
    return track;
}

int EraseGui::getErase()
{
    return erase;
}

int EraseGui::getType()
{
    return type;
}