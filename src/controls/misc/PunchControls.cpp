#include <controls/misc/PunchControls.hpp>

#include <ui/misc/PunchGui.hpp>
#include <ui/misc/TransGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::misc;
using namespace std;

PunchControls::PunchControls(mpc::Mpc* mpc)
	: AbstractMiscControls(mpc)
{
}

void PunchControls::turnWheel(int i)
{
    init();
    
	if (param.compare("autopunch") == 0) {
		punchGui->setAutoPunch(punchGui->getAutoPunch() + i);
	}
    //checkAllTimes(notch);
}

void PunchControls::function(int i)
{
    init();
	switch (i) {
    case 1:
        ls.lock()->openScreen("trans");
		transGui->setBar0(0);
        transGui->setBar1(sequencer.lock()->getActiveSequence().lock()->getLastBar());
        break;
    case 2:
		ls.lock()->openScreen("2ndseq");
        break;
    case 5:
        break;
    }
}

PunchControls::~PunchControls() {
}
