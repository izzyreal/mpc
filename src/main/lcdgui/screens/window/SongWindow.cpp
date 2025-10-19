#include "SongWindow.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/SongScreen.hpp>

#include <sequencer/Song.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

SongWindow::SongWindow(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "song-window", layerIndex)
{
}

void SongWindow::open()
{
	auto songNameRestLabel = findLabel("song-name-rest");
	auto defaultSongNameRestLabel = findLabel("default-name-rest");

	auto songNameFirstLetterField = findField("song-name-first-letter");
	auto defaultSongNameFirstLetterField = findField("default-name-first-letter");

	auto songScreen = mpc.screens->get<SongScreen>();
	auto song = sequencer.lock()->getSong(songScreen->activeSongIndex);

	songNameFirstLetterField->setText(song->getName().substr(0, 1));
	defaultSongNameFirstLetterField->setText(songScreen->defaultSongName.substr(0, 1));
	songNameRestLabel->setText(song->getName().substr(1));
	defaultSongNameRestLabel->setText(songScreen->defaultSongName.substr(1));
}

void SongWindow::function(int i)
{
	switch (i)
	{
	case 1:
		openScreen("delete-song");
		break;
	case 3:
		openScreen("song");
		break;
	case 4:
		openScreen("copy-song");
		break;
	}
}

void SongWindow::openNameScreen()
{
	init();


    std::function<void(std::string&)> enterAction;
    std::string initialNameScreenName;

    auto songScreen = mpc.screens->get<SongScreen>();

	if (param.find("default") != std::string::npos)
	{
		initialNameScreenName = songScreen->getDefaultSongName();
        
        enterAction = [songScreen, this](std::string& newName) {
            songScreen->setDefaultSongName(newName);
            openScreen(name);
        };
	}
	else
	{
        const auto songIndex = songScreen->getActiveSongIndex();
        const auto song = sequencer.lock()->getSong(songIndex);
		initialNameScreenName = song->getName();
        
        enterAction = [song, this](std::string& newName) {
            song->setName(newName);
            openScreen(name);
        };
	}

    auto nameScreen = mpc.screens->get<NameScreen>();
    nameScreen->initialize(initialNameScreenName, 16, enterAction, name);
	openScreen("name");
}
