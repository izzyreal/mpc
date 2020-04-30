#include "SequenceControls.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

SequenceControls::SequenceControls()
	: AbstractSequencerControls()
{
}

void SequenceControls::function(int i)
{
	super::function(i);
	switch (i) {
	case 1:
		ls.lock()->openScreen("deletesequence");
		break;
	case 4:
		ls.lock()->openScreen("copysequence");
		break;
	}
}

void SequenceControls::turnWheel(int i)
{
	init();
	auto seq = sequence.lock();
	if (param.find("default") != string::npos) {
		nameGui->setName(sequencer.lock()->getDefaultSequenceName());
	}
	else {
		nameGui->setName(seq->getName());
	}
	nameGui->setParameterName(param);
	ls.lock()->openScreen("name");
}
