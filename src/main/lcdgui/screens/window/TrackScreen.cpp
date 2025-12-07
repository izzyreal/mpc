#include "TrackScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/Label.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/window/NameScreen.hpp"

using namespace mpc::lcdgui::screens::window;

TrackScreen::TrackScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "track", layerIndex)
{
}

void TrackScreen::open()
{
    const auto activeTrackIndex = sequencer.lock()->getSelectedTrackIndex();
    const auto defaultTrackName =
        sequencer.lock()->getDefaultTrackName(activeTrackIndex);

    const auto track = sequencer.lock()->getSelectedTrack();
    findField("tracknamefirstletter")->setText(track->getName().substr(0, 1));
    findLabel("tracknamerest")->setText(track->getName().substr(1));

    findField("defaultnamefirstletter")->setText(defaultTrackName.substr(0, 1));
    findLabel("defaultnamerest")->setText(defaultTrackName.substr(1));
}

void TrackScreen::function(const int i)
{
    ScreenComponent::function(i);
    if (i == 1)
    {
        openScreenById(ScreenId::DeleteTrackScreen);
    }
    else if (i == 4)
    {
        openScreenById(ScreenId::CopyTrackScreen);
    }
}

void TrackScreen::openNameScreen()
{
    std::function<void(std::string &)> enterAction;
    std::string initialNameScreenName;

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("default") != std::string::npos)
    {
        initialNameScreenName = sequencer.lock()->getDefaultTrackName(
            sequencer.lock()->getSelectedTrackIndex());

        enterAction = [this](const std::string &nameScreenName)
        {
            sequencer.lock()->setDefaultTrackName(
                nameScreenName, sequencer.lock()->getSelectedTrackIndex());
            openScreenById(ScreenId::SequencerScreen);
        };
    }
    else
    {
        const auto track = sequencer.lock()->getSelectedTrack();

        initialNameScreenName = track->getName();

        enterAction = [this, track](const std::string &newName)
        {
            track->setName(newName);
            openScreenById(ScreenId::SequencerScreen);
        };
    }

    const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
    nameScreen->initialize(initialNameScreenName, 16, enterAction, "sequencer");
    openScreenById(ScreenId::NameScreen);
}
