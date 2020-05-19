#include <ui/sequencer/TrMoveObserver.hpp>

#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sequencer/TrMoveGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

TrMoveObserver::TrMoveObserver()
{
	
	sequencer = Mpc::instance().getSequencer();
	tmGui = Mpc::instance().getUis().lock()->getTrMoveGui();
	tmGui->addObserver(this);
	auto ls = Mpc::instance().getLayeredScreen().lock();
	sqField = ls->lookupField("sq");
	tr0Field = ls->lookupField("tr0");
	tr1Field = ls->lookupField("tr1");
	tr0Label = ls->lookupLabel("tr0info");
	tr1Label = ls->lookupLabel("tr1info");
	selectTrackLabel = ls->lookupLabel("selecttrack");
	toMoveLabel = ls->lookupLabel("tomove");
	selectTrackLabel.lock()->setText("Select track");
	toMoveLabel.lock()->setText("to move.");
	auto lSequencer = sequencer.lock();
	lSequencer->addObserver(this);

}

void TrMoveObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("sq") == 0) {
		displaySq();
	}
	else if (s.compare("trmove") == 0) {
		displayTrLabels();
		displayTrFields();
	}
	auto ls = Mpc::instance().getLayeredScreen().lock();
	ls->setCurrentBackground("trmove");
	selectTrackLabel.lock()->setText("Select track");
	toMoveLabel.lock()->setText("to move.");
	displaySq();
}

TrMoveObserver::~TrMoveObserver() {
	sequencer.lock()->deleteObserver(this);
	tmGui->deleteObserver(this);
}
