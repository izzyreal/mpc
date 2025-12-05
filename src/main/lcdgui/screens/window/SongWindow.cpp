#include "SongWindow.hpp"

#include "Mpc.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Song.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

SongWindow::SongWindow(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "song-window", layerIndex)
{
}

void SongWindow::open()
{
    const auto songNameRestLabel = findLabel("song-name-rest");
    const auto defaultSongNameRestLabel = findLabel("default-name-rest");

    const auto songNameFirstLetterField = findField("song-name-first-letter");
    const auto defaultSongNameFirstLetterField =
        findField("default-name-first-letter");

    const auto song = sequencer.lock()->getSelectedSong();

    songNameFirstLetterField->setText(song->getName().substr(0, 1));

    const auto songScreen = mpc.screens->get<ScreenId::SongScreen>();

    defaultSongNameFirstLetterField->setText(
        songScreen->getDefaultSongName().substr(0, 1));

    songNameRestLabel->setText(song->getName().substr(1));

    defaultSongNameRestLabel->setText(
        songScreen->getDefaultSongName().substr(1));
}

void SongWindow::function(const int i)
{
    switch (i)
    {
        case 1:
            openScreenById(ScreenId::DeleteSongScreen);
            break;
        case 3:
            openScreenById(ScreenId::SongScreen);
            break;
        case 4:
            openScreenById(ScreenId::CopySongScreen);
            break;
        default:;
    }
}

void SongWindow::openNameScreen()
{
    std::function<void(std::string &)> enterAction;
    std::string initialNameScreenName;

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.find("default") != std::string::npos)
    {
        auto songScreen = mpc.screens->get<ScreenId::SongScreen>();
        initialNameScreenName = songScreen->getDefaultSongName();

        enterAction = [songScreen, this](const std::string &newName)
        {
            songScreen->setDefaultSongName(newName);
            openScreenById(ScreenId::SongWindow);
        };
    }
    else
    {
        const auto song = sequencer.lock()->getSelectedSong();
        initialNameScreenName = song->getName();

        enterAction = [song, this](const std::string &newName)
        {
            song->setName(newName);
            openScreenById(ScreenId::SongWindow);
        };
    }

    const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
    nameScreen->initialize(initialNameScreenName, 16, enterAction,
                           "song-window");
    openScreenById(ScreenId::NameScreen);
}
