#include <ui/misc/PunchObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>

#include <lcdgui/Background.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/FunctionKeys.hpp>

#include <ui/misc/PunchGui.hpp>

#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/SeqUtil.hpp>

using namespace mpc::sequencer;
using namespace mpc::ui::misc;
using namespace std;

PunchObserver::PunchObserver()
{
	
	autoPunchNames = vector<string>{ "PUNCH IN ONLY", "PUNCH OUT ONLY", "PUNCH IN OUT" };
	punchGui = Mpc::instance().getUis().lock()->getPunchGui();
	punchGui->addObserver(this);
	auto ls = Mpc::instance().getLayeredScreen().lock();
	autoPunchField = ls->lookupField("autopunch");
	time0Field = ls->lookupField("time0");
	time1Field = ls->lookupField("time1");
	time2Field = ls->lookupField("time2");
	time3Field = ls->lookupField("time3");
	time4Field = ls->lookupField("time4");
	time5Field = ls->lookupField("time5");
	sequence = Mpc::instance().getSequencer().lock()->getActiveSequence().lock().get();
	displayBackground();
	displayAutoPunch();
	displayTime();
}

void PunchObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	string param = nonstd::any_cast<string>(a);
	if (param.compare("autopunch") == 0) {
		displayBackground();
		displayAutoPunch();
		displayTime();
	}
	else if (param.compare("time") == 0) {
		displayTime();
	}
}

void PunchObserver::displayAutoPunch()
{
	autoPunchField.lock()->setText(autoPunchNames[punchGui->getAutoPunch()]);
}

void PunchObserver::displayTime()
{
	auto ls = Mpc::instance().getLayeredScreen().lock();
	
	for (int i = 0; i < 3; i++)
	{
		ls->lookupField("time" + to_string(i)).lock()->Hide(punchGui->getAutoPunch() == 1);
		ls->lookupLabel("time" + to_string(i)).lock()->Hide(punchGui->getAutoPunch() == 1);
		ls->lookupField("time" + to_string(i + 3)).lock()->Hide(punchGui->getAutoPunch() == 0);
		ls->lookupLabel("time" + to_string(i + 3)).lock()->Hide(punchGui->getAutoPunch() == 0);
	}

	ls->lookupLabel("time3").lock()->Hide(punchGui->getAutoPunch() != 2);

    time0Field.lock()->setTextPadded(SeqUtil::getBar(sequence, punchGui->getTime0()) + 1, "0");
    time1Field.lock()->setTextPadded(SeqUtil::getBeat(sequence, punchGui->getTime0()) + 1, "0");
    time2Field.lock()->setTextPadded(SeqUtil::getClock(sequence, punchGui->getTime0()), "0");
    time3Field.lock()->setTextPadded(SeqUtil::getBar(sequence, punchGui->getTime1()) + 1, "0");
    time4Field.lock()->setTextPadded(SeqUtil::getBeat(sequence, punchGui->getTime1()) + 1, "0");
    time5Field.lock()->setTextPadded(SeqUtil::getClock(sequence, punchGui->getTime1()), "0");
}

void PunchObserver::displayBackground()
{
    string bgName = "punch";
	if (punchGui->getAutoPunch() == 1) {
		bgName = "punchout";
	}
	else if (punchGui->getAutoPunch() == 2) {
		bgName = "punchinout";
	}
	Mpc::instance().getLayeredScreen().lock()->getCurrentBackground()->setName(bgName);
	Mpc::instance().getLayeredScreen().lock()->lookupLabel("autopunch").lock()->SetDirty();
	Mpc::instance().getLayeredScreen().lock()->getFunctionKeys()->SetDirty();
}

PunchObserver::~PunchObserver() {
	punchGui->deleteObserver(this);
}
