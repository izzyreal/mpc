#include <controls/misc/TransControls.hpp>

#include <ui/misc/TransGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

using namespace mpc::controls::misc;
using namespace std;

TransControls::TransControls() 
	: AbstractMiscControls()
{
}

void TransControls::function(int i)
{
	init();
	switch (i) {
	case 0:
		ls.lock()->openScreen("punch");
		break;
	case 2:
		ls.lock()->openScreen("2ndseq");
		break;
	case 5:
		//if (transGui->getAmount() == 0) break; // does 2kxl do that?
		ls.lock()->openScreen("transposepermanent");
		break;
	}
}

void TransControls::turnWheel(int i)
{
	init();
	auto lSequencer = sequencer.lock();
	if (param.compare("tr") == 0) {
		transGui->setTr(transGui->getTr() + i);
	}
	else if (param.compare("transposeamount") == 0) {
		transGui->setAmount(transGui->getAmount() + i);
	}
	else if (param.compare("bar0") == 0) {
		auto candidate = transGui->getBar0() + i;
		if (candidate < 0 || candidate > lSequencer->getActiveSequence().lock()->getLastBar())
			return;

		transGui->setBar0(transGui->getBar0() + i);
	}
	else if (param.compare("bar1") == 0) {
		auto candidate = transGui->getBar1() + i;
		if (candidate < 0 || candidate > lSequencer->getActiveSequence().lock()->getLastBar())
			return;

		transGui->setBar1(transGui->getBar1() + i);
	}
}

TransControls::~TransControls() {
}
