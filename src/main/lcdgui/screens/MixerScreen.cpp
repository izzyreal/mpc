#include "MixerScreen.hpp"
#include "hardware2/Hardware2.h"

#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/Knob.hpp>

#include <sampler/Pad.hpp>

#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace mpc::sampler;
using namespace mpc::sequencer;
using namespace mpc::engine;

MixerScreen::MixerScreen(mpc::Mpc& mpc, const int layerIndex) 
: ScreenComponent(mpc, "mixer", layerIndex)
{
    selection.set(0);
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
        m->initLabels();
        m->setColors();
    }

    displayMixerStrips();
    
    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        mixerStrips[padIndex]->setSelection(selection[padIndex] ? yPos : -1);
    }

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
        mixerStrips.emplace_back(addChildT<MixerStrip>(mpc, i));
        mixerStrips.back()->setBank(mpc.getBank());
    }
    
    displayMixerStrips();
}

std::shared_ptr<StereoMixer> MixerScreen::getStereoMixerChannel(int index)
{
    const auto padIndex = index + (mpc.getBank() * 16);
    const auto pad = program->getPad(padIndex);
    const auto note = pad->getNote();
    
    if (note < 35 || note > 98)
        return {};
    
    auto noteParameters = dynamic_cast<NoteParameters*>(program->getNoteParameters(note));
    auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
    bool stereoMixSourceIsDrum = mixerSetupScreen->isStereoMixSourceDrum();
    
    return stereoMixSourceIsDrum ? activeDrum().getStereoMixerChannels()[note - 35] : noteParameters->getStereoMixerChannel();
}

std::shared_ptr<IndivFxMixer> MixerScreen::getIndivFxMixerChannel(int index)
{
    const auto padIndex = index + (mpc.getBank() * 16);
    const auto pad = program->getPad(padIndex);
    const auto note = pad->getNote();
    
    if (note < 35 || note > 98)
        return {};
    
    auto noteParameters = dynamic_cast<NoteParameters*>(program->getNoteParameters(note));
    auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
    bool indivFxSourceIsDrum = mixerSetupScreen->isIndivFxSourceDrum();
    
    return indivFxSourceIsDrum ? activeDrum().getIndivFxMixerChannels()[note - 35] : noteParameters->getIndivFxMixerChannel();
}

void MixerScreen::displayMixerStrip(int stripIndex)
{
    init();
    
    auto strip = mixerStrips[stripIndex];
    auto stereoMixer = getStereoMixerChannel(stripIndex);
    auto indivFxMixer = getIndivFxMixerChannel(stripIndex);

    strip->setBank(mpc.getBank());

    if (!stereoMixer || !indivFxMixer)
    {
        if (tab == 0)
            strip->findChild<Knob>("")->Hide(true);
        else
            strip->setValueAString("");
        
        strip->setValueB(0);
        return;
    }
    
    strip->findChild<Knob>("")->Hide(tab > 0);
    
    if (tab == 0)
    {
        strip->setValueA(stereoMixer->getPanning());
        strip->setValueB(stereoMixer->getLevel());
    }
    else if (tab == 1)
    {

        if (stripHasStereoSound(stripIndex))
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

    mixerStrips[stripIndex]->setSelection(selection[stripIndex] ? yPos : -1);
}

void MixerScreen::displayMixerStrips()
{
    for (int i = 0; i < 16; i++)
    {
        displayMixerStrip(i);
    }
}

void MixerScreen::update(Observable* o, Message message)
{
    const auto msg = std::get<std::string>(message);

    init();
    
    if (msg == "bank")
    {
        for (auto& m : mixerStrips)
            m->setBank(mpc.getBank());
        
        displayMixerStrips();
    }
}

void MixerScreen::displayFunctionKeys()
{
    if (tab == 0)
    {
        ls->setFunctionKeysArrangement(selection.count() > 1 ? 3 : 0);
    }
    else if (tab == 1)
    {
        ls->setFunctionKeysArrangement(selection.count() > 1 ? 4 : 1);
    }
    else if (tab == 2)
    {
        ls->setFunctionKeysArrangement(selection.count() > 1 ? 5 : 2);
    }
}

void MixerScreen::setLink(bool b)
{
    if (b)
    {
        selection.set();
    }
    else
    {
        selection.reset();
        selection.set(xPos);
    }
    
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
    
    for (int i = 0; i < 16; i ++)
    {
        mixerStrips[i]->setSelection(selection[i] ? yPos : -1);
    }

    displayFunctionKeys();
}

void MixerScreen::setTab(int i)
{
    tab = i;
    
    for (auto& m : mixerStrips)
    {
        m->initLabels();
        m->setColors();
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

    if (selection.all())
    {
        xPos = newXPos;
        return;
    }

    selection.reset();

    for (auto &m : mixerStrips)
    {
        m->setSelection(-1);
    }

    xPos = newXPos;
    
    selection.set(xPos);
    mixerStrips[xPos]->setSelection(yPos);
}

void MixerScreen::setYPos(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }
    
    yPos = i;
    
    for (int i = 0; i < 16; i++)
    {
        mixerStrips[i]->setSelection(selection[i] ? yPos :  -1);
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
    {
        return;
    }

    setXPos(xPos - 1);

    auto newPad = xPos + (mpc.getBank() * 16);
    mpc.setPad(newPad);
}

void MixerScreen::right()
{
    init();
    
    if (xPos >= 15)
    {
        return;
    }

    setXPos(xPos + 1);

    const auto newPadIndexWithBank = xPos + (mpc.getBank() * 16);
    mpc.setPad(newPadIndexWithBank);
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
            setLink(selection.count() == 1);
            break;
    }
}

void MixerScreen::turnWheel(int i)
{
    init();
    
    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        auto stereoMixer = getStereoMixerChannel(padIndex);
        auto indivFxMixer = getIndivFxMixerChannel(padIndex);
        
        if (!stereoMixer || !indivFxMixer)
        {
            // The selected pad's mixer strip is empty, because no note is assigned to the pad.
            continue;
        }
        
        if (tab == 0)
        {
            auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
            
            bool record = sequencer.lock()->isRecordingOrOverdubbing() && mixerSetupScreen->isRecordMixChangesEnabled();
            
            if (yPos == 0)
            {
                stereoMixer->setPanning(stereoMixer->getPanning() + i);
                
                if (record)
                {
                    recordMixerEvent(padIndex + (mpc.getBank() * 16), 1, stereoMixer->getPanning());
                }
                
                displayPanning();
            }
            else if (yPos == 1)
            {
                stereoMixer->setLevel(stereoMixer->getLevel() + i);
                
                if (record)
                {
                    recordMixerEvent(padIndex + (mpc.getBank() * 16), 0, stereoMixer->getLevel());
                }
                
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
}

void MixerScreen::recordMixerEvent(int pad, int param, int value)
{
    auto event = std::make_shared<MixerEvent>();
    sequencer.lock()->getActiveTrack()->addEvent(sequencer.lock()->getTickPosition(), event);
    event->setPadNumber(pad);
    event->setParameter(param);
    event->setValue(value);
}

void MixerScreen::displayStereoFaders()
{
    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto stereoMixer = getStereoMixerChannel(padIndex);
        const auto padWithBankIndex = padIndex + (mpc.getBank() * 16);
        const auto note = program->getNoteFromPad(padWithBankIndex);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);
        
        for (auto& p : padsWithSameNote)
        {
            auto strip = mixerStrips[p - (mpc.getBank() * 16)];
            
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
}

void MixerScreen::displayPanning()
{
    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto stereoMixer = getStereoMixerChannel(padIndex);
        const auto padIndexWithBank = padIndex + (mpc.getBank() * 16);
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);
        
        for (auto& p : padsWithSameNote)
        {
            auto strip = mixerStrips[p - (mpc.getBank() * 16)];
            
            if (p >= (mpc.getBank() * 16) && p < ((mpc.getBank() + 1) * 16))
            {
                if (!stereoMixer)
                {
                    strip->findChild<Knob>("")->Hide(true);
                    continue;
                }
                
                strip->setValueA(stereoMixer->getPanning());
            }
        }
    }
}

void MixerScreen::displayIndividualOutputs()
{
    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto stereoMixer = getStereoMixerChannel(padIndex);
        const auto indivFxMixer = getIndivFxMixerChannel(padIndex);
        
        const auto padIndexWithBank = padIndex + (mpc.getBank() * 16);
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);
        
        for (auto& p : padsWithSameNote)
        {
            const auto stripIndex = p - (mpc.getBank() * 16);
            auto strip = mixerStrips[stripIndex];
            
            if (p >= (mpc.getBank() * 16) && p < ((mpc.getBank() + 1) * 16))
            {
                if (!stereoMixer)
                {
                    strip->setValueAString("");
                    continue;
                }
                
                if (stripHasStereoSound(stripIndex))
                    strip->setValueAString(stereoNames[indivFxMixer->getOutput()]);
                else
                    strip->setValueAString(monoNames[indivFxMixer->getOutput()]);
            }
        }
    }
}

void MixerScreen::displayIndivFaders()
{
    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto indivFxMixer = getIndivFxMixerChannel(padIndex);
        const auto padIndexWithBank = padIndex + (mpc.getBank() * 16);
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);
        
        for (auto& p : padsWithSameNote)
        {
            auto strip = mixerStrips[p - (mpc.getBank() * 16)];
            
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
}

void MixerScreen::displayFxPaths()
{
    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }
    
        const auto indivFxMixer = getIndivFxMixerChannel(padIndex);
        const auto padIndexWithBank = padIndex + (mpc.getBank() * 16);
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);
        
        for (auto& p : padsWithSameNote)
        {
            auto strip = mixerStrips[p - (mpc.getBank() * 16)];
            
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
}

void MixerScreen::displayFxSendLevels()
{
    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto indivFxMixer = getIndivFxMixerChannel(padIndex);
        const auto padIndexWithBank = padIndex + (mpc.getBank() * 16);
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);
        
        for (auto& p : padsWithSameNote)
        {
            auto strip = mixerStrips[p - (mpc.getBank() * 16)];
            
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
}

bool MixerScreen::stripHasStereoSound(int stripIndex)
{
    auto pad = stripIndex + (mpc.getBank() * 16);
    auto note = program->getNoteFromPad(pad);
    auto noteParameters = program->getNoteParameters(note);
    auto soundIndex = noteParameters->getSoundIndex();
    return soundIndex != -1 && !sampler->getSound(soundIndex)->isMono();
}

void MixerScreen::pressPadIndexWithoutBank(const uint8_t padIndexWithoutBank)
{
    const bool shiftIsPressed = mpc.getHardware2()->getButton("shift")->isPressed();

    if (!shiftIsPressed)
    {
        selection.reset();
        selection.set(padIndexWithoutBank);
    }
    else
    {
        selection.flip(padIndexWithoutBank);
    }

    displayMixerStrips();
    displayFunctionKeys();
}

