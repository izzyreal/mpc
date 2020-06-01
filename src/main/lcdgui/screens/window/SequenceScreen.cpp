#include "SequenceScreen.hpp"

#include <ui/NameGui.hpp>

#include <lcdgui/Label.hpp>

using namespace mpc::lcdgui::screens::window;

SequenceScreen::SequenceScreen(const int layerIndex)
	: ScreenComponent("sequence", layerIndex)
{
}

void SequenceScreen::open()
{
	auto sequenceNameRestLabel = findLabel("sequencenamerest").lock();
	auto defaultSequenceNameRestLabel = findLabel("defaultnamerest").lock();

	auto sequenceNameFirstLetterField = findField("sequencenamefirstletter").lock();
	auto defaultSequenceNameFirstLetterField = findField("defaultnamefirstletter").lock();

	auto seq = sequencer.lock()->getActiveSequence().lock();

	sequenceNameFirstLetterField->setText(seq->getName().substr(0, 1));
	defaultSequenceNameFirstLetterField->setText(sequencer.lock()->getDefaultSequenceName().substr(0, 1));
	sequenceNameRestLabel->setText(seq->getName().substr(1, seq->getName().length()));
	defaultSequenceNameRestLabel->setText(sequencer.lock()->getDefaultSequenceName().substr(1, sequencer.lock()->getDefaultSequenceName().length()));
}

void SequenceScreen::function(int i)
{
	BaseControls::function(i);
	switch (i) {
	case 1:
		ls.lock()->openScreen("deletesequence");
		break;
	case 4:
		ls.lock()->openScreen("copysequence");
		break;
	}
}

void SequenceScreen::turnWheel(int i)
{
	init();
	auto nameGui = mpc.getUis().lock()->getNameGui();
	if (param.find("default") != string::npos) {
		nameGui->setName(sequencer.lock()->getDefaultSequenceName());
	}
	else {
		auto seq = sequencer.lock()->getActiveSequence().lock();
		nameGui->setName(seq->getName());
	}
	nameGui->setParameterName(param);
	ls.lock()->openScreen("name");
}
