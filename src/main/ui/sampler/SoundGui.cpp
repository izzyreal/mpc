#include "SoundGui.hpp"

#include <cmath>

using namespace mpc::ui::sampler;
using namespace std;

void SoundGui::setPlayX(int i)
{
	if (i < 0 || i > 4)
	{
		return;
	}
    playX = i;
    setChanged();
    notifyObservers(string("playx"));
}

int SoundGui::getPlayX()
{
    return playX;
}

void SoundGui::setView(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}
	view = i;
	setChanged();
	notifyObservers(string("view"));
}

int SoundGui::getView()
{
    return view;
}
