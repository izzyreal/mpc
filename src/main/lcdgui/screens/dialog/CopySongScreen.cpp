#include "CopySongScreen.hpp"

#include <sequencer/Song.hpp>
#include <lcdgui/screens/SongScreen.hpp>

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;

CopySongScreen::CopySongScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "copy-song", layerIndex)
{
}

void CopySongScreen::open()
{
	song1 = 0;

	for (int i = 0; i < 20; i++)
	{
		if (!sequencer.lock()->getSong(i)->isUsed())
		{
			song1 = i;
			break;
		}
	}

	displaySong0();
	displaySong1();
}

void CopySongScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		openScreen("song-window");
		break;
	case 4:
	{
		auto songScreen = mpc.screens->get<SongScreen>();
		sequencer.lock()->copySong(songScreen->activeSongIndex, song1);
		openScreen("song");
		break;
	}
	}
}

void CopySongScreen::turnWheel(int i)
{
	init();

	auto songScreen = mpc.screens->get<SongScreen>();

	if (param == "song0")
	{
		auto candidate = songScreen->activeSongIndex + i;

		if (candidate < 0) candidate = 0;
		if (candidate > 19) candidate = 19;

		songScreen->activeSongIndex = candidate;

		displaySong0();
	}
	else if (param == "song1")
	{
		setSong1(song1 + i);
	}
}

void CopySongScreen::setSong1(int i)
{
	if (i < 0 || i > 19)
		return;

	song1 = i;
	displaySong1();
}


void CopySongScreen::displaySong0()
{
	auto songScreen = mpc.screens->get<SongScreen>();
	auto song = sequencer.lock()->getSong(songScreen->activeSongIndex);
	findField("song0")->setText(StrUtil::padLeft(std::to_string(songScreen->activeSongIndex + 1), "0", 2) + "-" + song->getName());
}

void CopySongScreen::displaySong1()
{
	auto song = sequencer.lock()->getSong(song1);
	findField("song1")->setText(StrUtil::padLeft(std::to_string(song1 + 1), "0", 2) + "-" + song->getName());
}
