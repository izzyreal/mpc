#include <ui/sampler/MixerGui.hpp>

#include <lcdgui/MixerStrip.hpp>

using namespace mpc::ui::sampler;
using namespace std;

MixerGui::MixerGui()
{
	link = false;
}

void MixerGui::setLink(bool b)
{
    link = b;
    setChanged();
    notifyObservers(string("link"));
}

bool MixerGui::getLink()
{
    return link;
}

void MixerGui::setTab(int i)
{
    tab = i;
    setChanged();
    notifyObservers(string("tab"));
}

int MixerGui::getTab()
{
    return tab;
}

void MixerGui::setMixerStrips(vector<mpc::lcdgui::MixerStrip*> mixerStrips)
{
    this->mixerStrips = mixerStrips;
}

vector<mpc::lcdgui::MixerStrip*> MixerGui::getMixerStrips()
{
    return mixerStrips;
}

int MixerGui::getXPos()
{
    return xPos;
}

void MixerGui::setXPos(int i)
{
    if(i < 0 || i > 15) return;
	xPos = i;
    setChanged();
    notifyObservers(string("position"));
}

int MixerGui::getYPos()
{
    return yPos;
}

void MixerGui::setYPos(int i)
{
    if(i < 0 || i > 1) return;
    yPos = i;
    setChanged();
    notifyObservers(string("position"));
}

MixerGui::~MixerGui() {
}
