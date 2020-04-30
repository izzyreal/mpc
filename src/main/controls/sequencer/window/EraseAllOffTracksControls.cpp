#include <controls/sequencer/window/EraseAllOffTracksControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

EraseAllOffTracksControls::EraseAllOffTracksControls()
	: AbstractSequencerControls()
{
}

void EraseAllOffTracksControls::function(int i)
{
	super::function(i);
	auto seq = sequence.lock();
	int trackCounter;
	switch (i) {
	case 4:
		trackCounter = 0;
		for (auto& track : seq->getTracks()) {
			auto t = track.lock();
			if (!t->isOn()) {
				seq->purgeTrack(trackCounter);
			}
			trackCounter++;
		}
		ls.lock()->openScreen("sequencer");
		break;
	}
}
