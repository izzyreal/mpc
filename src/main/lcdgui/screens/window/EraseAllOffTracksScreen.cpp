#include <lcdgui/screens/window/EraseAllOffTracksScreen.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

EraseAllOffTracksScreen::EraseAllOffTracksScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "erase-all-off-tracks", layerIndex)
{
}

void EraseAllOffTracksScreen::function(int i)
{
	baseControls->function(i);

	switch (i)
	{
	case 4:
	{
		auto seq = sequencer.lock()->getActiveSequence().lock();
		int trackCounter = 0;

		for (auto& track : seq->getTracks())
		{
			auto t = track.lock();

			if (!t->isOn())
			{
				seq->purgeTrack(trackCounter);
			}
			trackCounter++;
		}
		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}
