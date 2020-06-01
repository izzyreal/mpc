#include "TrackScreen.hpp"

#include <sequencer/Track.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

TrackScreen::TrackScreen(const int layerIndex)
	: ScreenComponent("track", layerIndex)
{
}

void TrackScreen::open()
{
	init();

	auto activeTrackIndex = sequencer.lock()->getActiveTrackIndex();
	auto defaultTrackName = sequencer.lock()->getDefaultTrackName(activeTrackIndex);

	findField("tracknamefirstletter").lock()->setText(track.lock()->getName().substr(0, 1));
	findLabel("tracknamerest").lock()->setText(track.lock()->getName().substr(1, track.lock()->getName().length()));

	findField("defaultnamefirstletter").lock()->setText(defaultTrackName.substr(0, 1));
	findLabel("defaultnamerest").lock()->setText(defaultTrackName.substr(1, defaultTrackName.length()));
}

void TrackScreen::function(int i)
{
	BaseControls::function(i);
	switch (i)
	{
	case 1:
		ls.lock()->openScreen("deletetrack");
		break;
	case 4:
		ls.lock()->openScreen("copytrack");
		break;
	}
}

void TrackScreen::turnWheel(int i)
{
	init();
	
	if (param.find("default") != string::npos) {
		nameGui->setName(sequencer.lock()->getDefaultTrackName(sequencer.lock()->getActiveTrackIndex()));
	}
	else {
		nameGui->setName(track.lock()->getName());
	}
	nameGui->setParameterName(param);
	ls.lock()->openScreen("name");
}
