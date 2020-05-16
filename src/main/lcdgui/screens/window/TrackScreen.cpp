#include "TrackScreen.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

TrackScreen::TrackScreen(const int& layer)
	: ScreenComponent("track", layer)
{
}

void TrackScreen::function(int i)
{
	BaseControls::function(i);
	switch (i) {
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
