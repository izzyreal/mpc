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

    auto seq = sequencer.lock()->getActiveSequence();

    sequenceNameFirstLetterField->setText(seq->getName().substr(0, 1));
    defaultSequenceNameFirstLetterField->setText(sequencer.lock()->getDefaultSequenceName().substr(0, 1));
    sequenceNameRestLabel->setText(seq->getName().substr(1));
    defaultSequenceNameRestLabel->setText(sequencer.lock()->getDefaultSequenceName().substr(1));
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
    init();

    std::function<void(std::string&)> enterAction;
    std::string initialNameScreenName;

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName.find("default") != std::string::npos)
    {
        initialNameScreenName = sequencer.lock()->getDefaultSequenceName();

        enterAction = [this](std::string& nameScreenName) {
            sequencer.lock()->setDefaultSequenceName(nameScreenName);
            mpc.getLayeredScreen()->openScreen<SequenceScreen>();
        };
    }
    else
    {
        initialNameScreenName = sequencer.lock()->getActiveSequence()->getName();

        enterAction = [this](std::string& nameScreenName) {
            sequencer.lock()->getActiveSequence()->setName(nameScreenName);
            mpc.getLayeredScreen()->openScreen<SequenceScreen>();
        };
    }

    auto nameScreen = mpc.screens->get<NameScreen>();
    nameScreen->initialize(initialNameScreenName, 16, enterAction, "sequence");
    mpc.getLayeredScreen()->openScreen<NameScreen>();
}
