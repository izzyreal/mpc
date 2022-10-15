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

	auto seq = sequencer->getActiveSequence().lock();

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
    auto nameScreen = mpc.screens->get<NameScreen>("name");
    std::function<void(string&)> renamer;
    
    if (param.find("default") != string::npos)
    {
        nameScreen->setName(sequencer->getDefaultSequenceName());
        
        renamer = [&](string& newName) {
            sequencer->setDefaultSequenceName(newName);
        };
    }
    else
    {
        nameScreen->setName(sequencer->getActiveSequence().lock()->getName());
        
        renamer = [&](string& newName) {
            sequencer->getActiveSequence().lock()->setName(newName);
        };
    }

    nameScreen->setRenamerAndScreenToReturnTo(renamer, "sequence");
    openScreen("name");
}
