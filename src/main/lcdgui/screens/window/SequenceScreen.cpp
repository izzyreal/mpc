#include "SequenceScreen.hpp"

#include <lcdgui/Label.hpp>

using namespace mpc::lcdgui::screens::window;

SequenceScreen::SequenceScreen(const int& layer)
	: ScreenComponent("sequence", layer)
{
}

void SequenceScreen::open()
{
	auto sequenceNameRestLabel = findLabel("sequencenamerest").lock();
	auto defaultSequenceNameRestLabel = findLabel("defaultnamerest").lock();

	auto sequenceNameFirstLetterField = findField("sequencenamefirstletter").lock();
	auto defaultSequenceNameFirstLetterField = findField("defaultnamefirstletter").lock();

	auto seq = sequencer->getActiveSequence().lock();

	sequenceNameFirstLetterField->setText(seq->getName().substr(0, 1));
	defaultSequenceNameFirstLetterField->setText(sequencer->getDefaultSequenceName().substr(0, 1));
	sequenceNameRestLabel->setText(seq->getName().substr(1, seq->getName().length()));
	defaultSequenceNameRestLabel->setText(sequencer->getDefaultSequenceName().substr(1, sequencer->getDefaultSequenceName().length()));
}
