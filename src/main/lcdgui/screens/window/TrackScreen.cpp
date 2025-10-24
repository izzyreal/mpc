#include "TrackScreen.hpp"

#include "sequencer/Track.hpp"

#include "lcdgui/screens/window/NameScreen.hpp"

using namespace mpc::lcdgui::screens::window;

TrackScreen::TrackScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "track", layerIndex)
{
}

void TrackScreen::open()
{
	auto activeTrackIndex = sequencer.lock()->getActiveTrackIndex();
	auto defaultTrackName = sequencer.lock()->getDefaultTrackName(activeTrackIndex);

    auto track = mpc.getSequencer()->getActiveTrack();
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
        mpc.getLayeredScreen()->openScreen<DeleteTrackScreen>();
		break;
	case 4:
        mpc.getLayeredScreen()->openScreen<CopyTrackScreen>();
		break;
	}
}

void TrackScreen::openNameScreen()
{
    std::function<void(std::string&)> enterAction;
    std::string initialNameScreenName;

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("default") != std::string::npos)
    {
        initialNameScreenName = sequencer.lock()->getDefaultTrackName(sequencer.lock()->getActiveTrackIndex());

        enterAction = [this](std::string& nameScreenName) {
            sequencer.lock()->setDefaultTrackName(nameScreenName, sequencer.lock()->getActiveTrackIndex());
        mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        };
    }
    else
    {
        auto track = mpc.getSequencer()->getActiveTrack();

        if (!track->isUsed())
        {
            track->setUsed(true);
        }

        initialNameScreenName = track->getName();

        enterAction = [this, track](std::string& newName) {
            track->setName(newName);
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        };
    }

    auto nameScreen = mpc.screens->get<NameScreen>();
    nameScreen->initialize(initialNameScreenName, 16, enterAction, "sequencer");
    mpc.getLayeredScreen()->openScreen<NameScreen>();
}
