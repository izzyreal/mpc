#include "DeleteTrackScreen.hpp"

#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;
using namespace std;

DeleteTrackScreen::DeleteTrackScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-track", layerIndex)
{
}

void DeleteTrackScreen::open()
{
	setTr(sequencer->getActiveTrackIndex());
	displayTr();
}

void DeleteTrackScreen::turnWheel(int i)
{
	init();

	if (param.compare("tr") == 0)
		setTr(tr + i);
}

void DeleteTrackScreen::function(int i)
{
	init();

	switch (i)
	{
	case 2:
		openScreen("delete-all-tracks");
		break;
	case 3:
		openScreen("track");
		break;
	case 4:
	{
		auto s = sequencer->getActiveSequence().lock();
		s->purgeTrack(tr);
		openScreen("sequencer");
	}
	}
}

void DeleteTrackScreen::setTr(int i)
{
	if (i < 0 || i > 63)
		return;

	tr = i;
	displayTr();
}

void DeleteTrackScreen::displayTr()
{
	auto trackName = sequencer->getActiveSequence().lock()->getTrack(tr).lock()->getName();
	findField("tr").lock()->setText(StrUtil::padLeft(to_string(tr + 1), "0", 2) + "-" + trackName);
}

