#include "SequenceScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;

SequenceScreen::SequenceScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "sequence", layerIndex)
{
}

void SequenceScreen::open()
{
	auto sequenceNameRestLabel = findLabel("sequencenamerest");
	auto defaultSequenceNameRestLabel = findLabel("defaultnamerest");

	auto sequenceNameFirstLetterField = findField("sequencenamefirstletter");
	auto defaultSequenceNameFirstLetterField = findField("defaultnamefirstletter");

	auto seq = sequencer->getActiveSequence();

	sequenceNameFirstLetterField->setText(seq->getName().substr(0, 1));
	defaultSequenceNameFirstLetterField->setText(sequencer->getDefaultSequenceName().substr(0, 1));
	sequenceNameRestLabel->setText(seq->getName().substr(1));
	defaultSequenceNameRestLabel->setText(sequencer->getDefaultSequenceName().substr(1));
}

void SequenceScreen::function(int i)
{
	ScreenComponent::function(i);

	switch (i)
	{
	case 1:
		openScreen("delete-sequence");
		break;
	case 4:
		openScreen("copy-sequence");
		break;
	}
}

void SequenceScreen::turnWheel(int i)
{
    init();
    std::function<void(std::string&)> enterAction;
    std::string initialNameScreenName;
    
    if (param.find("default") != std::string::npos)
    {
        initialNameScreenName = sequencer->getDefaultSequenceName();
        
        enterAction = [this](std::string& nameScreenName) {
            sequencer->setDefaultSequenceName(nameScreenName);
            openScreen(name);
        };
    }
    else
    {
        initialNameScreenName = sequencer->getActiveSequence()->getName();
        
        enterAction = [this](std::string& nameScreenName) {
            sequencer->getActiveSequence()->setName(nameScreenName);
            openScreen(name);
        };
    }

    auto nameScreen = mpc.screens->get<NameScreen>("name");
    nameScreen->initialize(initialNameScreenName, 16, enterAction, name);
    openScreen("name");
}
