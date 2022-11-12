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
	auto songNameRestLabel = findLabel("song-name-rest").lock();
	auto defaultSongNameRestLabel = findLabel("default-name-rest").lock();

	auto songNameFirstLetterField = findField("song-name-first-letter").lock();
	auto defaultSongNameFirstLetterField = findField("default-name-first-letter").lock();

	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto song = sequencer->getSong(songScreen->activeSongIndex);

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

void SongWindow::turnWheel(int i)
{
	init();

	auto nameScreen = mpc.screens->get<NameScreen>("name");
    auto songScreen = mpc.screens->get<SongScreen>("song");

    std::function<void(std::string&)> renamer;

	if (param.find("default") != std::string::npos)
	{
		nameScreen->setName(songScreen->getDefaultSongName());
        
        renamer = [songScreen](std::string& newName) {
            songScreen->setDefaultSongName(newName);
        };
	}
	else
	{
        const auto songIndex = songScreen->getActiveSongIndex();
        const auto song = sequencer->getSong(songIndex);
		nameScreen->setName(song->getName());
        
        renamer = [song](std::string& newName) {
            song->setName(newName);
        };
	}

    nameScreen->setRenamerAndScreenToReturnTo(renamer, "song-window");
	openScreen("name");
}
