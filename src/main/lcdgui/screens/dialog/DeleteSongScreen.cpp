#include "DeleteSongScreen.hpp"

#include <lcdgui/screens/SongScreen.hpp>
#include <sequencer/Song.hpp>

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

DeleteSongScreen::DeleteSongScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-song", layerIndex)
{
}

void DeleteSongScreen::open()
{
	displaySong();
}

void DeleteSongScreen::turnWheel(int i)
{
	init();

	if (param == "song")
	{
		auto songScreen = mpc.screens->get<SongScreen>();
		auto candidate = songScreen->activeSongIndex + i;

		if (candidate < 0) candidate = 0;
		if (candidate > 19) candidate = 19;

		songScreen->activeSongIndex = candidate;

		displaySong();
	}
}

void DeleteSongScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 2:
		openScreen("delete-all-song");
		break;
	case 3:
		openScreen("song-window");
		break;
	case 4:
		auto songScreen = mpc.screens->get<SongScreen>();
		sequencer.lock()->deleteSong(songScreen->activeSongIndex);
		openScreen("song");
		break;
	}
}

void DeleteSongScreen::displaySong()
{
	auto songScreen = mpc.screens->get<SongScreen>();
	auto song = sequencer.lock()->getSong(songScreen->activeSongIndex);
	findField("song")->setText(StrUtil::padLeft(std::to_string(songScreen->activeSongIndex + 1), "0", 2) + "-" + song->getName());
}
