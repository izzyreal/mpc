#include "KeepOrRetryScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <sampler/NoteParameters.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace std;

KeepOrRetryScreen::KeepOrRetryScreen(mpc::Mpc& mpc, const int layerIndex) 
: ScreenComponent(mpc, "keep-or-retry", layerIndex)
{
}

void KeepOrRetryScreen::open()
{
    init();
    displayNameForNewSound();
    displayAssignToNote();
    mpc.addObserver(this); // Subscribe to "padandnote" message
}

void KeepOrRetryScreen::close()
{
    mpc.deleteObserver(this);
}

void KeepOrRetryScreen::mainScreen() {
    
    sampler.lock()->deleteSound(sampler.lock()->getPreviewSound());
    ScreenComponent::mainScreen();
}

void KeepOrRetryScreen::function(int i)
{
    init();
    
    switch (i)
    {
        case 1:
            sampler.lock()->deleteSound(sampler.lock()->getPreviewSound());
            openScreen("sample");
            break;
        case 3 :
            if (mpc.getControls().lock()->isF4Pressed())
            {
                return;
            }
            mpc.getControls().lock()->setF4Pressed(true);
            sampler.lock()->playPreviewSample(0, sampler.lock()->getPreviewSound().lock()->getLastFrameIndex(), 0, 2);
            break;
        case 4:
            auto index = sampler.lock()->getSoundCount() - 1;
            auto note = mpc.getNote();
            
            if (note != 34)
                sampler.lock()->getLastNp(program.lock().get())->setSoundIndex(index);
            
            sampler.lock()->setSoundIndex(index);
            openScreen("sample");
            break;
    }
}

void KeepOrRetryScreen::openNameScreen()
{
    auto nameScreen = mpc.screens->get<NameScreen>("name");
    nameScreen->setName(sampler.lock()->getPreviewSound().lock()->getName());
    auto _sampler = sampler.lock();
    
    auto renamer = [_sampler](string& newName) {
        _sampler->getPreviewSound().lock()->setName(newName);
    };
    
    nameScreen->setRenamerAndScreenToReturnTo(renamer, "keep-or-retry");
    
    openScreen("name");
}

void KeepOrRetryScreen::right()
{
    init();
    
    if (param.compare("name-for-new-sound") == 0)
        openNameScreen();
}

void KeepOrRetryScreen::turnWheel(int i)
{
    init();
    
    if (param.compare("name-for-new-sound") == 0)
    {
        openNameScreen();
    }
    else if (param.compare("assign-to-note") == 0)
    {
        auto note = mpc.getNote();
        
        if ( (note == 34 && i < 0) || (note == 98 && i > 0) )
        {
            return;
        }
        
        auto candidate = note + i;
        if (candidate < 34) candidate = 34;
        else if (candidate > 98) candidate = 98;
        
        auto pad = program.lock()->getPadIndexFromNote(candidate);
        
        mpc.setPadAndNote(pad, candidate);
    }
}

void KeepOrRetryScreen::displayNameForNewSound()
{
    if (!sampler.lock()->getSound().lock())
        return;
    
    findField("name-for-new-sound").lock()->setText(sampler.lock()->getPreviewSound().lock()->getName());
}

void KeepOrRetryScreen::displayAssignToNote()
{
    auto pad = mpc.getPad();
    auto note = mpc.getNote();
    auto noteStr = note == 34 ? "--" : to_string(note);
    auto padStr = pad == -1 ? "OFF" : sampler.lock()->getPadName(pad);
    findField("assign-to-note").lock()->setText(noteStr + "/" + padStr);
}

void KeepOrRetryScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
    string s = nonstd::any_cast<string>(arg);
    
    if (s.compare("padandnote") == 0)
        displayAssignToNote();
}
