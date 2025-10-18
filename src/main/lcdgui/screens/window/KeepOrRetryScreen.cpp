#include "KeepOrRetryScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>


using namespace mpc::lcdgui::screens::window;

KeepOrRetryScreen::KeepOrRetryScreen(mpc::Mpc& mpc, const int layerIndex) 
: ScreenComponent(mpc, "keep-or-retry", layerIndex)
{
}

void KeepOrRetryScreen::open()
{
    init();
    assignToNote = 34;
    displayNameForNewSound();
    displayAssignToNote();
    mpc.addObserver(this); // Subscribe to "note" message
}

void KeepOrRetryScreen::close()
{
    mpc.deleteObserver(this);
}

void KeepOrRetryScreen::function(int i)
{
    init();
    
    switch (i)
    {
        case 1:
            sampler->deleteSound(sampler->getPreviewSound());
            openScreen("sample");
            break;
        case 3 :
            sampler->playPreviewSample(0, sampler->getPreviewSound()->getLastFrameIndex(), 0);
            break;
        case 4:
            auto index = sampler->getSoundCount() - 1;

            if (assignToNote != 34)
            {
                program->getNoteParameters(assignToNote)->setSoundIndex(index);
            }

            sampler->setSoundIndex(index);
            openScreen("sample");
            break;
    }
}

void KeepOrRetryScreen::openNameScreen()
{
    init();

    if (param == "name-for-new-sound")
    {
        const auto enterAction = [this](std::string &nameScreenName) {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            sampler->getPreviewSound()->setName(nameScreenName);
            openScreen(name);
        };

        const auto nameScreen = mpc.screens->get<NameScreen>("name");
        nameScreen->initialize(sampler->getPreviewSound()->getName(), 16, enterAction, name);
        openScreen("name");
    }
}

void KeepOrRetryScreen::right()
{
    init();
    
    if (param == "name-for-new-sound")
        openNameScreen();
}

void KeepOrRetryScreen::turnWheel(int i)
{
    init();
    
    if (param == "assign-to-note")
    {
        auto newAssignToNote = assignToNote + i;

        if (newAssignToNote < 34)
        {
            newAssignToNote = 34;
        }
        else if (newAssignToNote > 98)
        {
            newAssignToNote = 98;
        }

        if (newAssignToNote == 34)
        {
            mpc.setNote(35);
            assignToNote = newAssignToNote;
            displayAssignToNote();
        }
        else
        {
            mpc.setNote(newAssignToNote);
        }
    }
}

void KeepOrRetryScreen::displayNameForNewSound()
{
    if (!sampler->getSound())
        return;
    
    findField("name-for-new-sound")->setText(sampler->getPreviewSound()->getName());
}

void KeepOrRetryScreen::displayAssignToNote()
{
    init();
    auto noteStr = assignToNote == 34 ? "--" : std::to_string(assignToNote);
    auto padStr = sampler->getPadName(program->getPadIndexFromNote(assignToNote));
    findField("assign-to-note")->setText(noteStr + "/" + padStr);
}

void KeepOrRetryScreen::update(Observable* o, Message message)
{
    const auto msg = std::get<std::string>(message);

    if (msg == "note")
    {
        assignToNote = mpc.getNote();
        displayAssignToNote();
    }
}
