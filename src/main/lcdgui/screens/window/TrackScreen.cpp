#include "TrackScreen.hpp"

#include <sequencer/Track.hpp>

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

TrackScreen::TrackScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "track", layerIndex)
{
}

void TrackScreen::open()
{
	ls.lock()->setPreviousScreenName("sequencer");

	init();
	auto activeTrackIndex = sequencer.lock()->getActiveTrackIndex();
	auto defaultTrackName = sequencer.lock()->getDefaultTrackName(activeTrackIndex);

	findField("tracknamefirstletter").lock()->setText(track.lock()->getName().substr(0, 1));
	findLabel("tracknamerest").lock()->setText(track.lock()->getName().substr(1));

	findField("defaultnamefirstletter").lock()->setText(defaultTrackName.substr(0, 1));
	findLabel("defaultnamerest").lock()->setText(defaultTrackName.substr(1));
}

void TrackScreen::function(int i)
{
	ScreenComponent::function(i);
	switch (i)
	{
	case 1:
		openScreen("delete-track");
		break;
	case 4:
		openScreen("copy-track");
		break;
	}
}

void TrackScreen::turnWheel(int i)
{
	init();
	auto nameScreen = mpc.screens->get<NameScreen>("name");
    std::function<void(string&)> renamer;
    
	if (param.find("default") != string::npos)
	{
        const auto _sequencer = sequencer.lock();
        nameScreen->setName(_sequencer->getDefaultTrackName(_sequencer->getActiveTrackIndex()));
        
        renamer = [_sequencer](string& newName) {
            _sequencer->setDefaultTrackName(newName, _sequencer->getActiveTrackIndex());
        };
	}
	else
	{
        const auto _track = track.lock();
		if (!_track->isUsed())
			_track->setUsed(true);

		nameScreen->setName(_track->getName());
        
        renamer = [_track](string& newName) {
            _track->setName(newName);
        };
	}

    nameScreen->setRenamerAndScreenToReturnTo(renamer, "sequencer");
    openScreen("name");
}
