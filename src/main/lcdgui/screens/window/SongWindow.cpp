#include "SongWindow.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/SongScreen.hpp>

#include <sequencer/Song.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace std;

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
	auto song = sequencer.lock()->getSong(songScreen->activeSongIndex).lock();

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

	if (param.find("default") != string::npos)
	{
		nameScreen->setName(songScreen->getDefaultSongName());
	}
	else
	{
		auto song = sequencer.lock()->getSong(songScreen->getActiveSongIndex()).lock();
		nameScreen->setName(song->getName());
	}

	nameScreen->parameterName = param;
	openScreen("name");
}
