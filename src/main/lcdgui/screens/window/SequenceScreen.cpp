#include "SequenceScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

SequenceScreen::SequenceScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "sequence", layerIndex)
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
	baseControls->function(i);

	switch (i)
	{
	case 1:
		ls.lock()->openScreen("delete-sequence");
		break;
	case 4:
		ls.lock()->openScreen("copy-sequence");
		break;
	}
}

void SequenceScreen::turnWheel(int i)
{
	init();

	auto nameScreen = dynamic_pointer_cast<NameScreen>(mpc.screens->getScreenComponent("name"));

	if (param.find("default") != string::npos)
	{
		nameScreen->setName(sequencer.lock()->getDefaultSequenceName());
	}
	else
	{
		auto seq = sequencer.lock()->getActiveSequence().lock();
		nameScreen->setName(seq->getName());
	}

	nameScreen->parameterName = param;
	ls.lock()->openScreen("name");
}
