#include <ui/sequencer/BarCopyObserver.hpp>
#include <Mpc.hpp>
#include <Util.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sequencer/BarCopyGui.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

BarCopyObserver::BarCopyObserver()
{
	bcGui = Mpc::instance().getUis().lock()->getBarCopyGui();
	bcGui->deleteObservers();
	bcGui->addObserver(this);
	auto ls = Mpc::instance().getLayeredScreen().lock();
	fromSqField = ls->lookupField("fromsq");
	toSqField = ls->lookupField("tosq");
	firstBarField = ls->lookupField("firstbar");
	lastBarField = ls->lookupField("lastbar");
	afterBarField = ls->lookupField("afterbar");
	copiesField = ls->lookupField("copies");
	displayFromSq();
	displayToSq();
	displayFirstBar();
	displayLastBar();
	displayAfterBar();
	displayCopies();
}

void BarCopyObserver::displayCopies()
{
    copiesField.lock()->setTextPadded(bcGui->getCopies(), " ");
}

void BarCopyObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("fromsq") == 0) {
		displayFromSq();
	}
	else if (s.compare("tosq") == 0) {
		displayToSq();
	}
	else if (s.compare("firstbar") == 0) {
		displayFirstBar();
	}
	else if (s.compare("lastbar") == 0) {
		displayLastBar();
	}
	else if (s.compare("afterbar") == 0) {
		displayAfterBar();
	}
	else if (s.compare("copies") == 0) {
		displayCopies();
	}
}

void BarCopyObserver::displayToSq()
{
    toSqField.lock()->setText(to_string(bcGui->getToSq() + 1));
}

void BarCopyObserver::displayFromSq()
{
    fromSqField.lock()->setText(to_string(bcGui->getFromSq() + 1));
}

void BarCopyObserver::displayAfterBar()
{
    afterBarField.lock()->setText(to_string(bcGui->getAfterBar()));
}

void BarCopyObserver::displayLastBar()
{
	lastBarField.lock()->setText(to_string(bcGui->getLastBar() + 1));
}

void BarCopyObserver::displayFirstBar()
{
	firstBarField.lock()->setText(to_string(bcGui->getFirstBar() + 1));
}

BarCopyObserver::~BarCopyObserver() {
	bcGui->deleteObserver(this);
}
