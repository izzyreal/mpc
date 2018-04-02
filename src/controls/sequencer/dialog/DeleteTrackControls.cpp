#include "DeleteTrackControls.hpp"

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer::dialog;
using namespace std;

DeleteTrackControls::DeleteTrackControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void DeleteTrackControls::turnWheel(int i)
{
	init();
	if (param.compare("tr") == 0) {
		swGui->setTrackNumber(swGui->getTrackNumber() + i);
	}
}

void DeleteTrackControls::function(int i)
{
	init();
	shared_ptr<mpc::sequencer::Sequence> s;
	switch (i) {
	case 2:
		ls.lock()->openScreen("deletealltracks");
		break;
	case 3:
		ls.lock()->openScreen("sequence");
		break;
	case 4:
		s = sequencer.lock()->getActiveSequence().lock();
		s->purgeTrack(swGui->getTrackNumber());
		ls.lock()->openScreen("sequencer");
	}
}
