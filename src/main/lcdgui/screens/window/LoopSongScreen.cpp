#include "LoopSongScreen.hpp"

#include <lcdgui/screens/SongScreen.hpp>

#include <sequencer/Song.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoopSongScreen::LoopSongScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "loop-song", layerIndex)
{
}

void LoopSongScreen::open()
{
	displayFirstStep();
	displayLastStep();
	displayNumberOfSteps();
}

void LoopSongScreen::turnWheel(int i)
{
	init();

	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto song = sequencer.lock()->getSong(songScreen->getActiveSongIndex());

	if (param == "first-step")
	{
		song->setFirstStep(song->getFirstStep() + i);
		displayFirstStep();
		displayLastStep();
		displayNumberOfSteps();
	}
	else if (param == "last-step")
	{
		song->setLastStep(song->getLastStep() + i);
		displayLastStep();
		displayFirstStep();
		displayNumberOfSteps();
	}
	else if (param == "number-of-steps")
	{
		auto candidate = song->getLastStep() + i;
		
		if (candidate < song->getFirstStep())
			return;

		song->setLastStep(candidate);

		displayLastStep();
		displayNumberOfSteps();
	}
}

void LoopSongScreen::displayFirstStep()
{
	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto song = sequencer.lock()->getSong(songScreen->getActiveSongIndex());
	findField("first-step")->setTextPadded(std::to_string(song->getFirstStep() + 1));
}

void LoopSongScreen::displayLastStep()
{
	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto song = sequencer.lock()->getSong(songScreen->getActiveSongIndex());

	findField("last-step")->setTextPadded(std::to_string(song->getLastStep() + 1));
}

void LoopSongScreen::displayNumberOfSteps()
{
	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto song = sequencer.lock()->getSong(songScreen->getActiveSongIndex());
	findField("number-of-steps")->setTextPadded(std::to_string(song->getLastStep() - song->getFirstStep() + 1));
}
