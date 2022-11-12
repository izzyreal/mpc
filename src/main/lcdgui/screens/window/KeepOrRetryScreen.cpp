#include "KeepOrRetryScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <sampler/NoteParameters.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;

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

void KeepOrRetryScreen::mainScreen() {
    
    sampler->deleteSound(sampler->getPreviewSound());
    ScreenComponent::mainScreen();
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
            if (mpc.getControls().lock()->isF4Pressed())
            {
                return;
            }
            sampler->playPreviewSample(0, sampler->getPreviewSound().lock()->getLastFrameIndex(), 0, 2);
            break;
        case 4:
            auto index = sampler->getSoundCount() - 1;
            sampler->getLastNp(program.lock().get())->setSoundIndex(index);
            sampler->setSoundIndex(index);
            openScreen("sample");
            break;
    }
}

void KeepOrRetryScreen::openNameScreen()
{
    auto nameScreen = mpc.screens->get<NameScreen>("name");
    nameScreen->setName(sampler->getPreviewSound().lock()->getName());
    auto _sampler = sampler;
    
    auto renamer = [_sampler](std::string& newName) {
        _sampler->getPreviewSound().lock()->setName(newName);
    };
    
    nameScreen->setRenamerAndScreenToReturnTo(renamer, "keep-or-retry");
    
    openScreen("name");
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
    
    if (param == "name-for-new-sound")
    {
        openNameScreen();
    }
    else if (param == "assign-to-note")
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
    if (!sampler->getSound().lock())
        return;
    
    findField("name-for-new-sound")->setText(sampler->getPreviewSound().lock()->getName());
}

void KeepOrRetryScreen::displayAssignToNote()
{
    init();
    auto noteStr = assignToNote == 34 ? "--" : std::to_string(assignToNote);
    auto padStr = sampler->getPadName(program.lock()->getPadIndexFromNote(assignToNote));
    findField("assign-to-note")->setText(noteStr + "/" + padStr);
}

void KeepOrRetryScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
    auto s = nonstd::any_cast<std::string>(arg);
    
    if (s == "note")
    {
        assignToNote = mpc.getNote();
        displayAssignToNote();
    }
}
