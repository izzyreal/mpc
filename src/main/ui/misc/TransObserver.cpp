#include <ui/misc/TransObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/misc/TransGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::ui::misc;
using namespace std;

TransObserver::TransObserver()
{
	
	transGui = Mpc::instance().getUis().lock()->getTransGui();
	transGui->addObserver(this);
	auto ls = Mpc::instance().getLayeredScreen().lock();
	trField = ls->lookupField("tr");
	trackNameLabel = ls->lookupLabel("trackname");
	transposeAmountField = ls->lookupField("transposeamount");
	bar0Field = ls->lookupField("bar0");
	bar1Field = ls->lookupField("bar1");
	displayTr();
	displayTransposeAmount();
	displayBars();
}

void TransObserver::displayTransposeAmount()
{
	transposeAmountField.lock()->setTextPadded(transGui->getAmount(), " ");
}

void TransObserver::displayTr()
{
	auto trName = string(transGui->getTr() == -1 ? "ALL" : Mpc::instance().getSequencer().lock()->getActiveSequence().lock()->getTrack(transGui->getTr()).lock()->getName());
	trField.lock()->setTextPadded(transGui->getTr() + 1, "0");
	trackNameLabel.lock()->setText(trName);
}

void TransObserver::displayBars()
{
    bar0Field.lock()->setText(to_string(transGui->getBar0() + 1));
    bar1Field.lock()->setText(to_string(transGui->getBar1() + 1));
}

void TransObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	string param = nonstd::any_cast<string>(a);
	if (param.compare("amount") == 0) {
		displayTransposeAmount();
	}
	else if (param.compare("tr") == 0) {
		displayTr();
	}
	else if (param.compare("bars") == 0) {
		displayBars();
	}
}

TransObserver::~TransObserver() {
	transGui->deleteObserver(this);
}
