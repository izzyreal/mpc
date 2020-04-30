#include <controls/sequencer/TrMuteControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <controls/Controls.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer;
using namespace std;

TrMuteControls::TrMuteControls()
	: AbstractSequencerControls()
{
}

void TrMuteControls::right()
{
}

void TrMuteControls::pad(int i, int velo, bool repeat, int tick)
{
	init();
	auto lSequencer = sequencer.lock();
	auto lLs = ls.lock();
	auto controls = Mpc::instance().getControls().lock();
	if (controls->isF6Pressed() || lSequencer->isSoloEnabled()) {
		if (!lSequencer->isSoloEnabled())
			lSequencer->setSoloEnabled(true);

		lSequencer->setSelectedTrackIndex(i + (bank_ * 16));
		lLs->removeCurrentBackground();
		lLs->setCurrentBackground("trackmutesolo2");
	}
	else {
		auto s = lSequencer->getActiveSequence().lock();
		auto t = s->getTrack(i + (bank_ * 16)).lock();
		t->setOn(!t->isOn());
	}
}

void TrMuteControls::turnWheel(int i)
{
	init();
	auto lSequencer = sequencer.lock();
	if (param.compare("sq") == 0) lSequencer->setActiveSequenceIndex(lSequencer->getActiveSequenceIndex() + i);
}

void TrMuteControls::function(int i)
{
	init();
	super::function(i);
	auto lSequencer = sequencer.lock();
	auto lLs = ls.lock();
	switch (i) {
	case 5:
		if (lSequencer->isSoloEnabled()) {
			lLs->setCurrentBackground("trackmute");
			lSequencer->setSoloEnabled(false);
		}
		else {
			lLs->setCurrentBackground("trackmutesolo1");
			lSequencer->setSoloEnabled(lSequencer->isSoloEnabled());
		}
		break;
	}
}
