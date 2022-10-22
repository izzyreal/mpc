#include "MixerScreen.hpp"

#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/Knob.hpp>

#include <sampler/Pad.hpp>

#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::sampler;
using namespace ctoot::mpc;
using namespace moduru::lang;
using namespace std;

MixerScreen::MixerScreen(mpc::Mpc& mpc, const int layerIndex) 
: ScreenComponent(mpc, "mixer", layerIndex)
{
    addMixerStrips();
}

void MixerScreen::open()
{
    if (lastTab != -1)
    {
        setTab(lastTab);
        lastTab = -1;
    }

    for (auto& m : mixerStrips)
    {
        m.lock()->initLabels();
        m.lock()->setColors();
    }

    displayMixerStrips();
    displayFunctionKeys();
    mpc.addObserver(this);
}

void MixerScreen::close()
{
    mpc.deleteObserver(this);
}

void MixerScreen::addMixerStrips()
{
    for (int i = 0; i < 16; i++)
    {
        mixerStrips.push_back(std::move(dynamic_pointer_cast<MixerStrip>(addChild(make_shared<MixerStrip>(mpc, i)).lock())));
        mixerStrips.back().lock()->setBank(mpc.getBank());
    }
    
    displayMixerStrips();
    mixerStrips[xPos].lock()->setSelection(yPos);
}

shared_ptr<MpcStereoMixerChannel> MixerScreen::getStereoMixerChannel(int index)
{
    const auto padIndex = index + (mpc.getBank() * 16);
    const auto pad = program.lock()->getPad(padIndex);
    const auto note = pad->getNote();
    
    if (note < 35 || note > 98)
        return {};
    
    auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
    auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
    bool stereoMixSourceIsDrum = mixerSetupScreen->isStereoMixSourceDrum();
    
    return stereoMixSourceIsDrum ? mpcSoundPlayerChannel->getStereoMixerChannels()[note - 35].lock() : noteParameters->getStereoMixerChannel().lock();
}

shared_ptr<MpcIndivFxMixerChannel> MixerScreen::getIndivFxMixerChannel(int index)
{
    const auto padIndex = index + (mpc.getBank() * 16);
    const auto pad = program.lock()->getPad(padIndex);
    const auto note = pad->getNote();
    
    if (note < 35 || note > 98)
        return {};
    
    auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
    auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
    bool indivFxSourceIsDrum = mixerSetupScreen->isIndivFxSourceDrum();
    
    return indivFxSourceIsDrum ? mpcSoundPlayerChannel->getIndivFxMixerChannels()[note - 35].lock() : noteParameters->getIndivFxMixerChannel().lock();
}

void MixerScreen::displayMixerStrip(int i)
{
    init();
    
    auto strip = mixerStrips[i].lock();
    auto stereoMixer = getStereoMixerChannel(i);
    auto indivFxMixer = getIndivFxMixerChannel(i);
    
    if (!stereoMixer || !indivFxMixer)
    {
        if (tab == 0)
            strip->findChild<Knob>("").lock()->Hide(true);
        else
            strip->setValueAString("");
        
        strip->setValueB(0);
        return;
    }
    
    strip->findChild<Knob>("").lock()->Hide(tab > 0);
    
    if (tab == 0)
    {
        strip->setValueA(stereoMixer->getPanning());
        strip->setValueB(stereoMixer->getLevel());
    }
    else if (tab == 1)
    {
        if (stereoMixer->isStereo())
            strip->setValueAString(stereoNames[indivFxMixer->getOutput()]);
        else
            strip->setValueAString(monoNames[indivFxMixer->getOutput()]);
        
        strip->setValueB(indivFxMixer->getVolumeIndividualOut());
    }
    else if (tab == 2)
    {
        strip->setValueAString(fxPathNames[indivFxMixer->getFxPath()]);
        strip->setValueB(indivFxMixer->getFxSendLevel());
    }

    mixerStrips[i].lock()->setSelection(xPos == i ? yPos : -1);
}

void MixerScreen::displayMixerStrips()
{
    for (int i = 0; i < 16; i++)
    {
        displayMixerStrip(i);
    }
}

void MixerScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
    string s = nonstd::any_cast<string>(arg);
    
    init();
    
    if (s == "bank")
    {
        for (auto& m : mixerStrips)
            m.lock()->setBank(mpc.getBank());
        
        displayMixerStrips();
    }
}

void MixerScreen::displayFunctionKeys()
{
    if (tab == 0)
    {
        ls.lock()->setFunctionKeysArrangement(link ? 3 : 0);
    }
    else if (tab == 1)
    {
        ls.lock()->setFunctionKeysArrangement(link ? 4 : 1);
    }
    else if (tab == 2)
    {
        ls.lock()->setFunctionKeysArrangement(link ? 5 : 2);
    }
}

void MixerScreen::setLink(bool b)
{
    link = b;
    
    if (tab == 0)
    {
        displayStereoFaders();
        displayPanning();
    }
    else if (tab == 1)
    {
        displayIndivFaders();
        displayIndividualOutputs();
    }
    else if (tab == 3)
    {
        displayFxSendLevels();
        displayFxPaths();
    }
    
    
    if (link)
    {
        for (auto& m : mixerStrips)
            m.lock()->setSelection(yPos);
    }
    else {
        for (auto& m : mixerStrips)
            m.lock()->setSelection(-1);
        
        mixerStrips[xPos].lock()->setSelection(yPos);
    }
    displayFunctionKeys();
}

void MixerScreen::setTab(int i)
{
    tab = i;
    
    for (auto& m : mixerStrips)
    {
        m.lock()->initLabels();
        m.lock()->setColors();
    }
    
    displayMixerStrips();
    displayFunctionKeys();
}

int MixerScreen::getTab()
{
    return tab;
}

void MixerScreen::setXPos(unsigned char newXPos)
{
    if (newXPos < 0 || newXPos > 15)
    {
        return;
    }

    xPos = newXPos;

    if (link)
    {
        for (auto& m : mixerStrips)
            m.lock()->setSelection(yPos);
    }
    else
    {
        for (auto& m : mixerStrips)
            m.lock()->setSelection(-1);

        mixerStrips[xPos].lock()->setSelection(yPos);
    }
}

void MixerScreen::setYPos(int i)
{
    if (i < 0 || i > 1)
        return;
    
    yPos = i;
    
    if (link)
    {
        for (auto& m : mixerStrips)
            m.lock()->setSelection(yPos);
    }
    else
    {
        for (auto& m : mixerStrips)
            m.lock()->setSelection(-1);
        
        mixerStrips[xPos].lock()->setSelection(yPos);
    }
}

void MixerScreen::up()
{
    setYPos(yPos - 1);
}

void MixerScreen::down()
{
    setYPos(yPos + 1);
}

void MixerScreen::left()
{
    init();
    
    if (xPos <= 0)
        return;

    setXPos(xPos - 1);

    auto newPad = xPos + (mpc.getBank() * 16);
    mpc.setPad(newPad);
}

void MixerScreen::right()
{
    init();
    
    if (xPos >= 15)
        return;

    setXPos(xPos + 1);

    auto newPad = xPos + (mpc.getBank() * 16);
    mpc.setPad(newPad);
}

void MixerScreen::openWindow()
{
    init();
    openScreen("channel-settings");
}

void MixerScreen::function(int f)
{
    init();
    
    switch (f)
    {
        case 0:
        case 1:
        case 2: // Intentional fall-through
            if (f == tab)
            {
                lastTab = tab;
                openScreen("select-mixer-drum");
            }
            else {
                setTab(f);
            }
            break;
        case 3:
            openScreen("mixer-setup");
            break;
            //	case 4:
            //openScreen("fx-edit"); // Not implemented
        case 5:
            setLink(!link);
            break;
    }
}

void MixerScreen::turnWheelLinked(int i)
{
    for (int j = 0; j < 16; j++)
    {
        auto stereoMixer = getStereoMixerChannel(j);
        auto indivFxMixer = getIndivFxMixerChannel(j);
        
        if (!stereoMixer || !indivFxMixer)
            continue;
        
        if (tab == 0)
        {
            auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
            bool record = sequencer->isRecordingOrOverdubbing() && mixerSetupScreen->isRecordMixChangesEnabled();
            
            if (yPos == 0)
            {
                if (!stereoMixer)
                    continue;
                
                stereoMixer->setPanning(stereoMixer->getPanning() + i);
                
                if (record)
                    recordMixerEvent(j + (mpc.getBank() * 16), 1, stereoMixer->getPanning());
            }
            else if (yPos == 1)
            {
                stereoMixer->setLevel(stereoMixer->getLevel() + i);
                
                if (record)
                    recordMixerEvent(j + (mpc.getBank() * 16), 0, stereoMixer->getLevel());
            }
        }
        else if (tab == 1)
        {
            if (yPos == 0)
                indivFxMixer->setOutput(indivFxMixer->getOutput() + i);
            else
                indivFxMixer->setVolumeIndividualOut(indivFxMixer->getVolumeIndividualOut() + i);
        }
        else if (tab == 2)
        {
            if (yPos == 0)
                indivFxMixer->setFxPath(indivFxMixer->getFxPath() + i);
            else
                indivFxMixer->setFxSendLevel(indivFxMixer->getFxSendLevel() + i);
        }
    }
    
    if (tab == 0)
    {
        displayPanning();
        displayStereoFaders();
    }
    else if (tab == 1)
    {
        displayIndividualOutputs();
        displayIndivFaders();
    }
    else if (tab == 2)
    {
        displayFxPaths();
        displayFxSendLevels();
    }
}

void MixerScreen::turnWheel(int i)
{
    init();
    
    if (link)
    {
        turnWheelLinked(i);
        return;
    }
    
    auto stereoMixer = getStereoMixerChannel(xPos);
    auto indivFxMixer = getIndivFxMixerChannel(xPos);
    
    if (!stereoMixer || !indivFxMixer)
    {
        // The selected pad's mixer strip is empty, because no note is assigned to the pad.
        return;
    }
    
    if (tab == 0)
    {
        auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
        
        bool record = sequencer->isRecordingOrOverdubbing() && mixerSetupScreen->isRecordMixChangesEnabled();
        
        if (yPos == 0)
        {
            stereoMixer->setPanning(stereoMixer->getPanning() + i);
            
            if (record)
                recordMixerEvent(xPos + (mpc.getBank() * 16), 1, stereoMixer->getPanning());
            
            displayPanning();
        }
        else if (yPos == 1)
        {
            stereoMixer->setLevel(stereoMixer->getLevel() + i);
            
            if (record)
                recordMixerEvent(xPos + (mpc.getBank() * 16), 0, stereoMixer->getLevel());
            
            displayStereoFaders();
        }
    }
    else if (tab == 1)
    {
        if (yPos == 0)
        {
            indivFxMixer->setOutput(indivFxMixer->getOutput() + i);
            displayIndividualOutputs();
        }
        else if (yPos == 1)
        {
            indivFxMixer->setVolumeIndividualOut(indivFxMixer->getVolumeIndividualOut() + i);
            displayIndivFaders();
        }
    }
    else if (tab == 2)
    {
        if (yPos == 0)
        {
            indivFxMixer->setFxPath(indivFxMixer->getFxPath() + i);
            displayFxPaths();
        }
        else if (yPos == 1)
        {
            indivFxMixer->setFxSendLevel(indivFxMixer->getFxSendLevel() + i);
            displayFxSendLevels();
        }
    }
}

void MixerScreen::recordMixerEvent(int pad, int param, int value)
{
    auto track = sequencer->getActiveTrack();
    auto e = dynamic_pointer_cast<mpc::sequencer::MixerEvent>(track->addEvent(sequencer->getTickPosition(), "mixer"));
    e->setPadNumber(pad);
    e->setParameter(param);
    e->setValue(value);
}

void MixerScreen::displayStereoFaders()
{
    if (link)
    {
        for (int i = 0; i < 16; i++)
        {
            auto strip = mixerStrips[i].lock();
            auto stereoMixer = getStereoMixerChannel(i);
            
            if (!stereoMixer)
            {
                strip->setValueB(0);
                continue;
            }
            
            strip->setValueB(stereoMixer->getLevel());
        }
        
        return;
    }
    
    auto stereoMixer = getStereoMixerChannel(xPos);
    auto pad = xPos + (mpc.getBank() * 16);
    auto note = program.lock()->getNoteFromPad(pad);
    auto padsWithSameNote = program.lock()->getPadIndicesFromNote(note);
    
    for (auto& p : padsWithSameNote)
    {
        auto strip = mixerStrips[p - (mpc.getBank() * 16)].lock();
        
        if (p >= (mpc.getBank() * 16) && p < ((mpc.getBank() + 1) * 16))
        {
            if (!stereoMixer)
            {
                strip->setValueB(0);
                continue;
            }
            
            strip->setValueB(stereoMixer->getLevel());
        }
    }
}

void MixerScreen::displayPanning()
{
    if (link)
    {
        for (int i = 0; i < 16; i++)
        {
            auto strip = mixerStrips[i].lock();
            auto stereoMixer = getStereoMixerChannel(i);
            
            if (!stereoMixer)
            {
                strip->findChild<Knob>("").lock()->Hide(true);
                continue;
            }
            
            strip->setValueA(stereoMixer->getPanning());
        }
        
        return;
    }
    
    auto stereoMixer = getStereoMixerChannel(xPos);
    auto pad = xPos + (mpc.getBank() * 16);
    auto note = program.lock()->getNoteFromPad(pad);
    auto padsWithSameNote = program.lock()->getPadIndicesFromNote(note);
    
    for (auto& p : padsWithSameNote)
    {
        auto strip = mixerStrips[p - (mpc.getBank() * 16)].lock();
        
        if (p >= (mpc.getBank() * 16) && p < ((mpc.getBank() + 1) * 16))
        {
            if (!stereoMixer)
            {
                strip->findChild<Knob>("").lock()->Hide(true);
                continue;
            }
            
            strip->setValueA(stereoMixer->getPanning());
        }
    }
}

void MixerScreen::displayIndividualOutputs()
{
    if (link)
    {
        for (int i = 0; i < 16; i++)
        {
            auto strip = mixerStrips[i].lock();
            auto stereoMixer = getStereoMixerChannel(i);
            auto indivFxMixer = getIndivFxMixerChannel(i);
            
            if (!stereoMixer || !indivFxMixer)
            {
                strip->setValueAString("");
                continue;
            }
            
            if (stereoMixer->isStereo())
                strip->setValueAString(stereoNames[indivFxMixer->getOutput()]);
            else
                strip->setValueAString(monoNames[indivFxMixer->getOutput()]);
        }
        
        return;
    }
    
    auto stereoMixer = getStereoMixerChannel(xPos);
    auto indivFxMixer = getIndivFxMixerChannel(xPos);
    
    auto pad = xPos + (mpc.getBank() * 16);
    auto note = program.lock()->getNoteFromPad(pad);
    auto padsWithSameNote = program.lock()->getPadIndicesFromNote(note);
    
    for (auto& p : padsWithSameNote)
    {
        auto strip = mixerStrips[p - (mpc.getBank() * 16)].lock();
        
        if (p >= (mpc.getBank() * 16) && p < ((mpc.getBank() + 1) * 16))
        {
            if (!stereoMixer)
            {
                strip->setValueAString("");
                continue;
            }
            
            if (stereoMixer->isStereo())
                strip->setValueAString(stereoNames[indivFxMixer->getOutput()]);
            else
                strip->setValueAString(monoNames[indivFxMixer->getOutput()]);
        }
    }
}

void MixerScreen::displayIndivFaders()
{
    if (link)
    {
        for (int i = 0; i < 16; i++)
        {
            auto strip = mixerStrips[i].lock();
            auto indivFxMixer = getIndivFxMixerChannel(i);
            
            if (!indivFxMixer)
            {
                strip->setValueB(0);
                continue;
            }
            
            strip->setValueB(indivFxMixer->getVolumeIndividualOut());
        }
        
        return;
    }
    
    auto indivFxMixer = getIndivFxMixerChannel(xPos);
    auto pad = xPos + (mpc.getBank() * 16);
    auto note = program.lock()->getNoteFromPad(pad);
    auto padsWithSameNote = program.lock()->getPadIndicesFromNote(note);
    
    for (auto& p : padsWithSameNote)
    {
        auto strip = mixerStrips[p - (mpc.getBank() * 16)].lock();
        
        if (p >= (mpc.getBank() * 16) && p < ((mpc.getBank() + 1) * 16))
        {
            if (!indivFxMixer)
            {
                strip->setValueB(0);
                continue;
            }
            
            strip->setValueB(indivFxMixer->getVolumeIndividualOut());
        }
    }
}

void MixerScreen::displayFxPaths()
{
    if (link)
    {
        for (int i = 0; i < 16; i++)
        {
            auto strip = mixerStrips[i].lock();
            auto indivFxMixer = getIndivFxMixerChannel(i);
            
            if (!indivFxMixer)
            {
                strip->setValueAString("");
                continue;
            }
            
            strip->setValueAString(fxPathNames[indivFxMixer->getFxPath()]);
        }
        
        return;
    }
    
    auto indivFxMixer = getIndivFxMixerChannel(xPos);
    auto pad = xPos + (mpc.getBank() * 16);
    auto note = program.lock()->getNoteFromPad(pad);
    auto padsWithSameNote = program.lock()->getPadIndicesFromNote(note);
    
    for (auto& p : padsWithSameNote)
    {
        auto strip = mixerStrips[p - (mpc.getBank() * 16)].lock();
        
        if (p >= (mpc.getBank() * 16) && p < ((mpc.getBank() + 1) * 16))
        {
            if (!indivFxMixer)
            {
                strip->setValueAString("");
                continue;
            }
            
            strip->setValueAString(fxPathNames[indivFxMixer->getFxPath()]);
        }
    }
}

void MixerScreen::displayFxSendLevels()
{
    if (link)
    {
        for (int i = 0; i < 16; i++)
        {
            auto strip = mixerStrips[i].lock();
            auto indivFxMixer = getIndivFxMixerChannel(i);
            
            if (!indivFxMixer)
            {
                strip->setValueB(0);
                continue;
            }
            
            strip->setValueB(indivFxMixer->getFxSendLevel());
        }
        
        return;
    }
    
    auto indivFxMixer = getIndivFxMixerChannel(xPos);
    auto pad = xPos + (mpc.getBank() * 16);
    auto note = program.lock()->getNoteFromPad(pad);
    auto padsWithSameNote = program.lock()->getPadIndicesFromNote(note);
    
    for (auto& p : padsWithSameNote)
    {
        auto strip = mixerStrips[p - (mpc.getBank() * 16)].lock();
        
        if (p >= (mpc.getBank() * 16) && p < ((mpc.getBank() + 1) * 16))
        {
            if (!indivFxMixer)
            {
                strip->setValueB(0);
                continue;
            }
            
            strip->setValueB(indivFxMixer->getFxSendLevel());
        }
    }
}
