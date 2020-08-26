#include "LoopSongScreen.hpp"

#include <lcdgui/screens/SongScreen.hpp>

#include <sequencer/Song.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace std;

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
	auto song = sequencer.lock()->getSong(songScreen->getActiveSongIndex()).lock();

	if (param.compare("first-step") == 0)
	{
		song->setFirstStep(song->getFirstStep() + i);
		displayFirstStep();
		displayLastStep();
		displayNumberOfSteps();
	}
	else if (param.compare("last-step") == 0)
	{
		song->setLastStep(song->getLastStep() + i);
		displayLastStep();
		displayFirstStep();
		displayNumberOfSteps();
	}
	else if (param.compare("number-of-steps") == 0)
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
	auto song = sequencer.lock()->getSong(songScreen->getActiveSongIndex()).lock();
	findField("first-step").lock()->setTextPadded(to_string(song->getFirstStep() + 1));
}

void LoopSongScreen::displayLastStep()
{
	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto song = sequencer.lock()->getSong(songScreen->getActiveSongIndex()).lock();

	findField("last-step").lock()->setTextPadded(to_string(song->getLastStep() + 1));
}

void LoopSongScreen::displayNumberOfSteps()
{
	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto song = sequencer.lock()->getSong(songScreen->getActiveSongIndex()).lock();
	findField("number-of-steps").lock()->setTextPadded(to_string(song->getLastStep() - song->getFirstStep() + 1));
}
