#include "DeleteTrackScreen.hpp"

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

DeleteTrackScreen::DeleteTrackScreen(const int layer)
	: ScreenComponent("deletetrack", layer)
{
}

void DeleteTrackScreen::open()
{
	displayTrackNumber();
}

void DeleteTrackScreen::turnWheel(int i)
{
	init();
	if (param.compare("tr") == 0)
	{
		setTrackNumber(trackNumber + i);
	}
}

void DeleteTrackScreen::function(int i)
{
	init();
	shared_ptr<mpc::sequencer::Sequence> s;
	switch (i)
	{
	case 2:
		ls.lock()->openScreen("deletealltracks");
		break;
	case 3:
		ls.lock()->openScreen("sequence");
		break;
	case 4:
		s = sequencer.lock()->getActiveSequence().lock();
		s->purgeTrack(trackNumber);
		ls.lock()->openScreen("sequencer");
	}
}

void DeleteTrackScreen::setTrackNumber(int i)
{
	if (i < 0 || i > 63)
	{
		return;
	}
	trackNumber = i;
	displayTrackNumber();
}

void DeleteTrackScreen::displayTrackNumber()
{
	auto trackName = sequencer.lock()->getActiveSequence().lock()->getTrack(trackNumber).lock()->getName();
	findField("tr").lock()->setText(moduru::lang::StrUtil::padLeft(to_string(trackNumber + 1), " ", 2) + "-" + trackName);
}

