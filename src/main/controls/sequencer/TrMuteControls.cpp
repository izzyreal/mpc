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
	
	auto lLs = ls.lock();
	auto controls = Mpc::instance().getControls().lock();
	if (controls->isF6Pressed() || sequencer.lock()->isSoloEnabled()) {
		if (!sequencer.lock()->isSoloEnabled())
			sequencer.lock()->setSoloEnabled(true);

		sequencer.lock()->setSelectedTrackIndex(i + (bank_ * 16));
		lLs->removeCurrentBackground();
		lLs->setCurrentBackground("trackmutesolo2");
	}
	else {
		auto s = sequencer.lock()->getActiveSequence().lock();
		auto t = s->getTrack(i + (bank_ * 16)).lock();
		t->setOn(!t->isOn());
	}
}

void TrMuteControls::turnWheel(int i)
{
	init();
	
	if (param.compare("sq") == 0) sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
}

void TrMuteControls::function(int i)
{
	init();
	super::function(i);
	
	auto lLs = ls.lock();
	switch (i) {
	case 5:
		if (sequencer.lock()->isSoloEnabled()) {
			lLs->setCurrentBackground("trackmute");
			sequencer.lock()->setSoloEnabled(false);
		}
		else {
			lLs->setCurrentBackground("trackmutesolo1");
			sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());
		}
		break;
	}
}
