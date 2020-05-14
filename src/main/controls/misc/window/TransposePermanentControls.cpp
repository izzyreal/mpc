#include <controls/misc/window/TransposePermanentControls.hpp>

#include <ui/misc/TransGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

using namespace mpc::controls::misc::window;
using namespace std;

TransposePermanentControls::TransposePermanentControls()
	: AbstractMiscControls()
{
}

void TransposePermanentControls::function(int i)
{
	super::function(i);
	init();
	switch (i) {
	case 4:
		auto all = transGui->getTr() == -1;
		vector<int> tracks;
		if (all) {
			for (int i = 0; i < 64; i++)
				tracks.push_back(i);
		}
		else {
			tracks.push_back(transGui->getTr());
		}
		auto seq = sequencer.lock()->getActiveSequence().lock();
		int firstTick = seq->getFirstTickOfBar(transGui->getBar0());
		int lastTick = seq->getLastTickOfBar(transGui->getBar1());
		
		for (auto& i : tracks) {
			auto t = seq->getTrack(i);
			for (auto& n : t.lock()->getNoteEvents()) {
				auto noteEvent = n.lock();
				if (noteEvent->getTick() < firstTick || noteEvent->getTick() > lastTick) continue;
				noteEvent->setNote(noteEvent->getNote() + transGui->getAmount());
			}
		}
		ls.lock()->openScreen("trans");
		break;
	}
}

void TransposePermanentControls::turnWheel(int i)
{
	init();
	
	if (param.compare("tr") == 0) {
		transGui->setTr(transGui->getTr() + i);
	}
	else if (param.compare("transposeamount") == 0) {
		transGui->setAmount(transGui->getAmount() + i);
	}
	else if (param.compare("bar0") == 0) {
		auto candidate = transGui->getBar0() + i;
		if (candidate < 0 || candidate > sequencer.lock()->getActiveSequence().lock()->getLastBar())
			return;

		transGui->setBar0(transGui->getBar0() + i);
	}
	else if (param.compare("bar1") == 0) {
		auto candidate = transGui->getBar1() + i;
		if (candidate < 0 || candidate > sequencer.lock()->getActiveSequence().lock()->getLastBar())
			return;

		transGui->setBar1(transGui->getBar1() + i);
	}
}

TransposePermanentControls::~TransposePermanentControls() {
}
