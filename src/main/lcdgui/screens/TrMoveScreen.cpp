#include "TrMoveScreen.hpp"

#include <ui/sequencer/BarCopyGui.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/sequencer/TrMoveGui.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

TrMoveScreen::TrMoveScreen(const int& layer)
	: ScreenComponent("trmove", layer)
{
	tmGui = Mpc::instance().getUis().lock()->getTrMoveGui();
}

void TrMoveScreen::open()
{
	displaySq();
	displayTrFields();
	displayTrLabels();
}

void TrMoveScreen::init()
{
    super::init();
    seq = sequencer.lock()->getSequence(tmGui->getSq());
}

void TrMoveScreen::turnWheel(int i)
{
	init();
	if (param.find("tr") != string::npos && i > 0) {
		tmGui->goUp();
	}
	else if (param.find("tr") != string::npos && i < 0) {
		tmGui->goDown();
	}
	else if (param.compare("sq") == 0) {
		tmGui->setSq(tmGui->getSq() + i);
	}
}

void TrMoveScreen::up()
{
	init();
	if (param.find("tr") != string::npos) tmGui->goUp();
}

void TrMoveScreen::down()
{
	init();
	if (param.find("tr") != string::npos) tmGui->goDown();
}

void TrMoveScreen::left()
{
	init();
	if (tmGui->isSelected() && param.compare("tr0") == 0) {
		return;
	}
	else if (!tmGui->isSelected() && param.compare("sq") == 0) {
		return;
	}
	super::left();
	init();
	if (param.compare("sq") == 0) {
		ls.lock()->drawFunctionKeys("trmove");
	}
}

void TrMoveScreen::right()
{
	init();
	if (tmGui->isSelected() && param.compare("tr0") == 0) {
		return;
	}
	if (!tmGui->isSelected() && param.compare("tr1") == 0) {
		return;
	}
	super::right();
	init();
	if (param.compare("sq") != 0 && !tmGui->isSelected()) {
		ls.lock()->drawFunctionKeys("trmove_notselected");
	}
}

void TrMoveScreen::function(int i)
{
	init();
	switch (i) {
	case 0:
		Mpc::instance().getUis().lock()->getEditSequenceGui()->setFromSq(tmGui->getSq());
		ls.lock()->openScreen("edit");
		break;
	case 1:
		Mpc::instance().getUis().lock()->getBarCopyGui()->setFromSq(tmGui->getSq());
		ls.lock()->openScreen("barcopy");
		break;
	case 3:
		ls.lock()->openScreen("user");
		break;
	case 4:
		if (tmGui->isSelected()) tmGui->cancel();
		ls.lock()->setFocus("tr1");
		break;
	case 5:
		if (param.compare("sq") == 0) break;
		if (tmGui->isSelected()) {
			tmGui->insert(seq.lock().get());
			ls.lock()->setFocus("tr1");
			ls.lock()->drawFunctionKeys("trmove_notselected");
		}
		else {
			tmGui->select();
			ls.lock()->setFocus("tr0");
		}
		break;
	}
}


void TrMoveScreen::displayTrLabels()
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
		findLabel("tr0").lock()->Hide(true);
	}
	else {
		findLabel("tr0").lock()->Hide(false);
		findLabel("tr0").lock()->setText(tr0);
	}
	if (tr1.compare("") == 0) {
		findLabel("tr1").lock()->Hide(true);
	}
	else {
		findLabel("tr1").lock()->Hide(false);
		findLabel("tr1").lock()->setText(tr1);
	}
}

void TrMoveScreen::displayTrFields()
{
	auto s = sequencer.lock()->getSequence(tmGui->getSq()).lock();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	if (tmGui->isSelected()) {
		selectTrackLabel.lock()->Hide(true);
		toMoveLabel.lock()->Hide(true);
		findField("tr1").lock()->Hide(true);
		findField("tr0").lock()->Hide(false);
		auto tr0Name = s->getTrack(tmGui->getSelectedTrackIndex()).lock()->getName();
		if (tr0Name.length() < 10) tr0Name = moduru::lang::StrUtil::padRight(tr0Name, " ", 9) + u8"\u00CD";
		findField("tr0").lock()->setText("Tr:" + moduru::lang::StrUtil::padLeft(to_string(tmGui->getSelectedTrackIndex() + 1), "0", 2) + "-" + tr0Name);
		ls->drawFunctionKeys("trmove_selected");
	}
	else {
		selectTrackLabel.lock()->Hide(false);
		toMoveLabel.lock()->Hide(false);
		findField("tr0").lock()->Hide(true);
		findField("tr1").lock()->Hide(false);
		findField("tr1").lock()->setText("Tr:" + moduru::lang::StrUtil::padLeft(to_string(tmGui->getCurrentTrackIndex() + 1), "0", 2) + "-" + s->getTrack(tmGui->getCurrentTrackIndex()).lock()->getName());
		if (ls->getFocus().compare(findField("tr1").lock()->getName()) == 0) {
			ls->drawFunctionKeys("trmove_notselected");
		}
		else {
			ls->drawFunctionKeys("trmove");
		}
	}
}

void TrMoveScreen::displaySq()
{
	Mpc::instance().getLayeredScreen().lock()->lookupLabel("sq").lock()->SetDirty();
	sqField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(tmGui->getSq() + 1), "0", 2) + "-" + sequencer.lock()->getSequence(tmGui->getSq()).lock()->getName());
}
