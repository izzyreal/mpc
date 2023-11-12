#include "TrackScreen.hpp"

#include <sequencer/Track.hpp>

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;

TrackScreen::TrackScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "track", layerIndex)
{
}

void TrackScreen::open()
{
	ls->setPreviousScreenName("sequencer");

	init();
	auto activeTrackIndex = sequencer.lock()->getActiveTrackIndex();
	auto defaultTrackName = sequencer.lock()->getDefaultTrackName(activeTrackIndex);

	findField("tracknamefirstletter")->setText(track->getName().substr(0, 1));
	findLabel("tracknamerest")->setText(track->getName().substr(1));

	findField("defaultnamefirstletter")->setText(defaultTrackName.substr(0, 1));
	findLabel("defaultnamerest")->setText(defaultTrackName.substr(1));
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

void TrackScreen::openNameScreen()
{
    init();
    std::function<void(std::string&)> enterAction;
    std::string initialNameScreenName;

    if (param.find("default") != std::string::npos)
    {
        initialNameScreenName = sequencer.lock()->getDefaultTrackName(sequencer.lock()->getActiveTrackIndex());

        enterAction = [this](std::string& nameScreenName) {
            sequencer.lock()->setDefaultTrackName(nameScreenName, sequencer.lock()->getActiveTrackIndex());
            openScreen("sequencer");
        };
    }
    else
    {
        if (!track->isUsed())
        {
            track->setUsed(true);
        }

        initialNameScreenName = track->getName();

        enterAction = [this](std::string& newName) {
            track->setName(newName);
            openScreen("sequencer");
        };
    }

    auto nameScreen = mpc.screens->get<NameScreen>("name");
    nameScreen->initialize(initialNameScreenName, 16, enterAction, "sequencer");
    openScreen("name");
}