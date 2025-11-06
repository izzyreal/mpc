#include "SequenceScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc::lcdgui::screens::window;

SequenceScreen::SequenceScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "sequence", layerIndex)
{
}

void SequenceScreen::open()
{
    const auto sequenceNameRestLabel = findLabel("sequencenamerest");
    const auto defaultSequenceNameRestLabel = findLabel("defaultnamerest");

    const auto sequenceNameFirstLetterField =
        findField("sequencenamefirstletter");
    const auto defaultSequenceNameFirstLetterField =
        findField("defaultnamefirstletter");

    const auto seq = sequencer->getActiveSequence();

    sequenceNameFirstLetterField->setText(seq->getName().substr(0, 1));
    defaultSequenceNameFirstLetterField->setText(
        sequencer->getDefaultSequenceName().substr(0, 1));
    sequenceNameRestLabel->setText(seq->getName().substr(1));
    defaultSequenceNameRestLabel->setText(
        sequencer->getDefaultSequenceName().substr(1));
}

void SequenceScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 1:
            openScreenById(ScreenId::DeleteSequenceScreen);
            break;
        case 4:
            openScreenById(ScreenId::CopySequenceScreen);
            break;
    }
}

void SequenceScreen::openNameScreen()
{
    std::function<void(std::string &)> enterAction;
    std::string initialNameScreenName;

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName.find("default") != std::string::npos)
    {
        initialNameScreenName = sequencer->getDefaultSequenceName();

        enterAction = [this](const std::string &nameScreenName)
        {
            sequencer->setDefaultSequenceName(nameScreenName);
            openScreenById(ScreenId::SequenceScreen);
        };
    }
    else
    {
        initialNameScreenName = sequencer->getActiveSequence()->getName();

        enterAction = [this](const std::string &nameScreenName)
        {
            sequencer->getActiveSequence()->setName(nameScreenName);
            openScreenById(ScreenId::SequenceScreen);
        };
    }

    const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
    nameScreen->initialize(initialNameScreenName, 16, enterAction, "sequence");
    openScreenById(ScreenId::NameScreen);
}
