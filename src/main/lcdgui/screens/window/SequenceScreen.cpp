#include "SequenceScreen.hpp"

#include "lcdgui/Label.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

using namespace mpc::lcdgui::screens::window;

SequenceScreen::SequenceScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "sequence", layerIndex)
{
}

void SequenceScreen::open()
{
    auto sequenceNameRestLabel = findLabel("sequencenamerest");
    auto defaultSequenceNameRestLabel = findLabel("defaultnamerest");

    auto sequenceNameFirstLetterField = findField("sequencenamefirstletter");
    auto defaultSequenceNameFirstLetterField =
        findField("defaultnamefirstletter");

    auto seq = sequencer->getActiveSequence();

    sequenceNameFirstLetterField->setText(seq->getName().substr(0, 1));
    defaultSequenceNameFirstLetterField->setText(
        sequencer->getDefaultSequenceName().substr(0, 1));
    sequenceNameRestLabel->setText(seq->getName().substr(1));
    defaultSequenceNameRestLabel->setText(
        sequencer->getDefaultSequenceName().substr(1));
}

void SequenceScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 1:
            mpc.getLayeredScreen()->openScreen<DeleteSequenceScreen>();
            break;
        case 4:
            mpc.getLayeredScreen()->openScreen<CopySequenceScreen>();
            break;
    }
}

void SequenceScreen::openNameScreen()
{

    std::function<void(std::string &)> enterAction;
    std::string initialNameScreenName;

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("default") != std::string::npos)
    {
        initialNameScreenName = sequencer->getDefaultSequenceName();

        enterAction = [this](std::string &nameScreenName)
        {
            sequencer->setDefaultSequenceName(nameScreenName);
            mpc.getLayeredScreen()->openScreen<SequenceScreen>();
        };
    }
    else
    {
        initialNameScreenName = sequencer->getActiveSequence()->getName();

        enterAction = [this](std::string &nameScreenName)
        {
            sequencer->getActiveSequence()->setName(nameScreenName);
            mpc.getLayeredScreen()->openScreen<SequenceScreen>();
        };
    }

    auto nameScreen = mpc.screens->get<NameScreen>();
    nameScreen->initialize(initialNameScreenName, 16, enterAction, "sequence");
    mpc.getLayeredScreen()->openScreen<NameScreen>();
}
