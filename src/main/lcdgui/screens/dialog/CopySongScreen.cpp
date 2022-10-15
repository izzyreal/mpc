#include "CopySongScreen.hpp"

#include <sequencer/Song.hpp>
#include <lcdgui/screens/SongScreen.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;
using namespace std;

CopySongScreen::CopySongScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "copy-song", layerIndex)
{
}

void CopySongScreen::open()
{
	song1 = 0;

	for (int i = 0; i < 20; i++)
	{
		if (!sequencer->getSong(i).lock()->isUsed())
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
		auto songScreen = mpc.screens->get<SongScreen>("song");
		sequencer->copySong(songScreen->activeSongIndex, song1);
		openScreen("song");
		break;
	}
	}
}

void CopySongScreen::turnWheel(int i)
{
	init();

	auto songScreen = mpc.screens->get<SongScreen>("song");

	if (param.compare("song0") == 0)
	{
		auto songScreen = mpc.screens->get<SongScreen>("song");
		auto candidate = songScreen->activeSongIndex + i;

		if (candidate < 0) candidate = 0;
		if (candidate > 19) candidate = 19;

		songScreen->activeSongIndex = candidate;

		displaySong0();
	}
	else if (param.find("1") != string::npos)
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
	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto song = sequencer->getSong(songScreen->activeSongIndex).lock();
	findField("song0").lock()->setText(StrUtil::padLeft(to_string(songScreen->activeSongIndex + 1), "0", 2) + "-" + song->getName());
}

void CopySongScreen::displaySong1()
{
	auto song = sequencer->getSong(song1).lock();
	findField("song1").lock()->setText(StrUtil::padLeft(to_string(song1 + 1), "0", 2) + "-" + song->getName());
}
