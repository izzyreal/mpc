#include <ui/sequencer/TrMoveObserver.hpp>

#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sequencer/TrMoveGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

TrMoveObserver::TrMoveObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	sequencer = mpc->getSequencer();
	tmGui = mpc->getUis().lock()->getTrMoveGui();
	tmGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
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
	displaySq();
	displayTrFields();
	displayTrLabels();
}

void TrMoveObserver::displayTrLabels()
{
	auto s = sequencer.lock()->getSequence(tmGui->getSq()).lock();
	string tr0 = "";
	string tr1 = "";
	string tr0Name = "";
	string tr1Name = "";
	auto tr0Index = tmGui->getCurrentTrackIndex() - 1;
	auto tr1Index = 0;
	if (tmGui->isSelected() && tr0Index >= tmGui->getSelectedTrackIndex())
		tr0Index++;

	tr1Index = tr0Index + 2;
	if (tmGui->isSelected()) {
		tr1Index--;
		if (tr0Index == tmGui->getSelectedTrackIndex()) {
			tr0Index--;
		}
		if (tr1Index == tmGui->getSelectedTrackIndex()) {
			tr1Index++;
		}
	}
	if (tr0Index >= 0) {
		tr0Name = s->getTrack(tr0Index).lock()->getName();
		tr0 += "Tr:" + moduru::lang::StrUtil::padLeft(to_string(tr0Index + 1), "0", 2) + "-" + tr0Name;
	}
	else {
		tr0 = "";
	}
	if (tr1Index < 64) {
		tr1Name = s->getTrack(tr1Index).lock()->getName();
		tr1 += "Tr:" + moduru::lang::StrUtil::padLeft(to_string(tr1Index + 1), "0", 2) + "-" + tr1Name;
	}
	else {
		tr1 = "";
	}
	if (tr0.compare("") == 0) {
		tr0Label.lock()->Hide(true);
	}
	else {
		tr0Label.lock()->Hide(false);
		tr0Label.lock()->setText(tr0);
	}
	if (tr1.compare("") == 0) {
		tr1Label.lock()->Hide(true);
	}
	else {
		tr1Label.lock()->Hide(false);
		tr1Label.lock()->setText(tr1);
	}
}

void TrMoveObserver::displayTrFields()
{
	auto s = sequencer.lock()->getSequence(tmGui->getSq()).lock();
	auto ls = mpc->getLayeredScreen().lock();
	if (tmGui->isSelected()) {
		selectTrackLabel.lock()->Hide(true);
		toMoveLabel.lock()->Hide(true);
		tr1Field.lock()->Hide(true);
		tr0Field.lock()->Hide(false);
		auto tr0Name = s->getTrack(tmGui->getSelectedTrackIndex()).lock()->getName();
		if (tr0Name.length() < 10) tr0Name = moduru::lang::StrUtil::padRight(tr0Name, " ", 9) + u8"\u00CD";
		tr0Field.lock()->setText("Tr:" + moduru::lang::StrUtil::padLeft(to_string(tmGui->getSelectedTrackIndex() + 1), "0", 2) + "-" + tr0Name);
		ls->drawFunctionKeys("trmove_selected");
	}
	else {
		selectTrackLabel.lock()->Hide(false);
		toMoveLabel.lock()->Hide(false);
		tr0Field.lock()->Hide(true);
		tr1Field.lock()->Hide(false);
		tr1Field.lock()->setText("Tr:" + moduru::lang::StrUtil::padLeft(to_string(tmGui->getCurrentTrackIndex() + 1), "0", 2) + "-" + s->getTrack(tmGui->getCurrentTrackIndex()).lock()->getName());
		if (ls->getFocus().compare(tr1Field.lock()->getName()) == 0) {
			ls->drawFunctionKeys("trmove_notselected");
		}
		else {
			ls->drawFunctionKeys("trmove");
		}
	}
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
	auto ls = mpc->getLayeredScreen().lock();
	ls->setCurrentBackground("trmove");
	selectTrackLabel.lock()->setText("Select track");
	toMoveLabel.lock()->setText("to move.");
	displaySq();
}

void TrMoveObserver::displaySq()
{
	mpc->getLayeredScreen().lock()->lookupLabel("sq").lock()->SetDirty();
	sqField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(tmGui->getSq() + 1), "0", 2) + "-" + sequencer.lock()->getSequence(tmGui->getSq()).lock()->getName());
}

TrMoveObserver::~TrMoveObserver() {
	sequencer.lock()->deleteObserver(this);
	tmGui->deleteObserver(this);
}
