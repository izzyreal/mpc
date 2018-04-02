#include <ui/misc/PunchObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/Background.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/misc/PunchGui.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::ui::misc;
using namespace std;

PunchObserver::PunchObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	autoPunchNames = vector<string>{ "PUNCH IN ONLY", "PUNCH OUT ONLY", "PUNCH IN OUT" };
	punchGui = mpc->getUis().lock()->getPunchGui();
	punchGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	autoPunchField = ls->lookupField("autopunch");
	time0Field = ls->lookupField("time0");
	time1Field = ls->lookupField("time1");
	time2Field = ls->lookupField("time2");
	time3Field = ls->lookupField("time3");
	time4Field = ls->lookupField("time4");
	time5Field = ls->lookupField("time5");
	sequence = mpc->getSequencer().lock()->getActiveSequence().lock().get();
	displayBackground();
	displayAutoPunch();
	displayTime();
}

void PunchObserver::update(moduru::observer::Observable* o, boost::any a)
{
	string param = boost::any_cast<string>(a);
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
	auto ls = mpc->getLayeredScreen().lock();
	for (int i = 0; i < 3; i++) {
		ls->lookupField("time" + to_string(i)).lock()->Hide(punchGui->getAutoPunch() == 1);
		ls->lookupLabel("time" + to_string(i)).lock()->Hide(punchGui->getAutoPunch() == 1);
		ls->lookupField("time" + to_string(i + 3)).lock()->Hide(punchGui->getAutoPunch() == 0);
		ls->lookupLabel("time" + to_string(i + 3)).lock()->Hide(punchGui->getAutoPunch() == 0);
	}
	ls->lookupLabel("time3").lock()->Hide(punchGui->getAutoPunch() != 2);
    time0Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getBarNumber(sequence, punchGui->getTime0()) + 1, "0");
    time1Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getBeatNumber(sequence, punchGui->getTime0()) + 1, "0");
    time2Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getClockNumber(sequence, punchGui->getTime0()), "0");
    time3Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getBarNumber(sequence, punchGui->getTime1()) + 1, "0");
    time4Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getBeatNumber(sequence, punchGui->getTime1()) + 1, "0");
    time5Field.lock()->setTextPadded(mpc::ui::sequencer::EditSequenceGui::getClockNumber(sequence, punchGui->getTime1()), "0");
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
	mpc->getLayeredScreen().lock()->getCurrentBackground()->setName(bgName);
	mpc->getLayeredScreen().lock()->lookupLabel("autopunch").lock()->SetDirty();
	mpc->getLayeredScreen().lock()->getFunctionKeys()->SetDirty();
}

PunchObserver::~PunchObserver() {
	punchGui->deleteObserver(this);
}
