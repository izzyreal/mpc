#include "MixerScreen.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "hardware/Hardware.hpp"

#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "lcdgui/Knob.hpp"

#include "sampler/Pad.hpp"

#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/MixerEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::sampler;
using namespace mpc::sequencer;
using namespace mpc::engine;

MixerScreen::MixerScreen(Mpc &mpc, const int layerIndex)
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

    for (const auto &m : mixerStrips)
    {
        m->initLabels();
        m->setColors();
    }

    displayMixerStrips();

    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        mixerStrips[padIndex]->setSelection(selection[padIndex] ? yPos : -1);
    }

    mpc.clientEventController->addObserver(this);
}

void MixerScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void MixerScreen::addMixerStrips()
{
    for (int i = 0; i < 16; i++)
    {
        mixerStrips.emplace_back(addChildT<MixerStrip>(mpc, i));
        mixerStrips.back()->setBank(0);
    }
}

std::shared_ptr<StereoMixer> MixerScreen::getStereoMixerChannel(int index) const
{
    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    const auto padIndex = index + bank * 16;
    const auto program = getProgramOrThrow();
    const auto pad = program->getPad(padIndex);
    const auto note = pad->getNote();

    if (note < 35 || note > 98)
    {
        return {};
    }

    const auto noteParameters = program->getNoteParameters(note);
    const auto mixerSetupScreen =
        mpc.screens->get<ScreenId::MixerSetupScreen>();
    const bool stereoMixSourceIsDrum =
        mixerSetupScreen->isStereoMixSourceDrum();

    return stereoMixSourceIsDrum
               ? getActiveDrumBus()->getStereoMixerChannels()[note - 35]
               : noteParameters->getStereoMixerChannel();
}

std::shared_ptr<IndivFxMixer>
MixerScreen::getIndivFxMixerChannel(int index) const
{
    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    const auto padIndex = index + bank * 16;
    const auto program = getProgramOrThrow();
    const auto pad = program->getPad(padIndex);
    const auto note = pad->getNote();

    if (note < 35 || note > 98)
    {
        return {};
    }

    const auto noteParameters = program->getNoteParameters(note);
    const auto mixerSetupScreen =
        mpc.screens->get<ScreenId::MixerSetupScreen>();
    const bool indivFxSourceIsDrum = mixerSetupScreen->isIndivFxSourceDrum();

    return indivFxSourceIsDrum
               ? getActiveDrumBus()->getIndivFxMixerChannels()[note - 35]
               : noteParameters->getIndivFxMixerChannel();
}

void MixerScreen::displayMixerStrip(int stripIndex)
{

    const auto strip = mixerStrips[stripIndex];
    const auto stereoMixer = getStereoMixerChannel(stripIndex);
    const auto indivFxMixer = getIndivFxMixerChannel(stripIndex);

    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());

    strip->setBank(bank);

    if (!stereoMixer || !indivFxMixer)
    {
        if (tab == 0)
        {
            strip->findChild<Knob>("")->Hide(true);
        }
        else
        {
            strip->setValueAString("");
        }

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
        {
            strip->setValueAString(stereoNames[indivFxMixer->getOutput()]);
        }
        else
        {
            strip->setValueAString(monoNames[indivFxMixer->getOutput()]);
        }

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

void MixerScreen::update(Observable *o, Message message)
{
    const auto msg = std::get<std::string>(message);
    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());

    if (msg == "bank")
    {
        for (const auto &m : mixerStrips)
        {
            m->setBank(bank);
        }

        displayMixerStrips();
    }
}

void MixerScreen::displayFunctionKeys() const
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

    for (int i = 0; i < 16; i++)
    {
        mixerStrips[i]->setSelection(selection[i] ? yPos : -1);
    }

    displayFunctionKeys();
}

void MixerScreen::setTab(int i)
{
    tab = i;

    for (const auto &m : mixerStrips)
    {
        m->initLabels();
        m->setColors();
    }

    displayMixerStrips();
    displayFunctionKeys();
}

int MixerScreen::getTab() const
{
    return tab;
}

void MixerScreen::setXPos(unsigned char newXPos)
{
    if (selection.all())
    {
        xPos = std::clamp(static_cast<int>(newXPos), 0, 15);
        return;
    }

    selection.reset();

    for (const auto &m : mixerStrips)
    {
        m->setSelection(-1);
    }

    xPos = std::clamp(static_cast<int>(newXPos), 0, 15);

    selection.set(xPos);
    mixerStrips[xPos]->setSelection(yPos);
}

void MixerScreen::setYPos(int i)
{
    yPos = std::clamp(i, 0, 1);

    for (int i = 0; i < 16; i++)
    {
        mixerStrips[i]->setSelection(selection[i] ? yPos : -1);
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

    if (xPos <= 0)
    {
        return;
    }

    setXPos(xPos - 1);

    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    const auto newPad = xPos + bank * 16;
    mpc.clientEventController->setSelectedPad(newPad);
}

void MixerScreen::right()
{

    if (xPos >= 15)
    {
        return;
    }

    setXPos(xPos + 1);

    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    const auto newPadIndexWithBank = xPos + bank * 16;
    mpc.clientEventController->setSelectedPad(newPadIndexWithBank);
}

void MixerScreen::openWindow()
{
    openScreenById(ScreenId::ChannelSettingsScreen);
}

void MixerScreen::function(int f)
{

    switch (f)
    {
        case 0:
        case 1:
        case 2: // Intentional fall-through
            if (f == tab)
            {
                lastTab = tab;
                openScreenById(ScreenId::SelectMixerDrumScreen);
            }
            else
            {
                setTab(f);
            }
            break;
        case 3:
            openScreenById(ScreenId::MixerSetupScreen);
            break;
            //	case 4:
            // openScreen<FxEditScreen>(); // Not implemented
        case 5:
            setLink(selection.count() == 1);
            break;
    }
}

void MixerScreen::turnWheel(int i)
{

    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto stereoMixer = getStereoMixerChannel(padIndex);
        const auto indivFxMixer = getIndivFxMixerChannel(padIndex);

        if (!stereoMixer || !indivFxMixer)
        {
            // The selected pad's mixer strip is empty, because no note is
            // assigned to the pad.
            continue;
        }

        if (tab == 0)
        {
            const auto mixerSetupScreen =
                mpc.screens->get<ScreenId::MixerSetupScreen>();

            const bool record =
                sequencer->getTransport()->isRecordingOrOverdubbing() &&
                mixerSetupScreen->isRecordMixChangesEnabled();

            const int bank =
                static_cast<int>(mpc.clientEventController->getActiveBank());

            if (yPos == 0)
            {
                stereoMixer->setPanning(stereoMixer->getPanning() + i);

                if (record)
                {
                    recordMixerEvent(padIndex + bank * 16, 1,
                                     stereoMixer->getPanning());
                }

                displayPanning();
            }
            else if (yPos == 1)
            {
                stereoMixer->setLevel(stereoMixer->getLevel() + i);

                if (record)
                {
                    recordMixerEvent(padIndex + bank * 16, 0,
                                     stereoMixer->getLevel());
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
                indivFxMixer->setVolumeIndividualOut(
                    indivFxMixer->getVolumeIndividualOut() + i);
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
                indivFxMixer->setFxSendLevel(indivFxMixer->getFxSendLevel() +
                                             i);
                displayFxSendLevels();
            }
        }
    }
}

void MixerScreen::recordMixerEvent(int pad, int param, int value) const
{
    const auto event = std::make_shared<MixerEvent>();
    sequencer->getActiveTrack()->addEvent(
        sequencer->getTransport()->getTickPosition(), event);
    event->setPadNumber(pad);
    event->setParameter(param);
    event->setValue(value);
}

void MixerScreen::displayStereoFaders()
{
    const auto program = getProgramOrThrow();

    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());

    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto stereoMixer = getStereoMixerChannel(padIndex);
        const auto padWithBankIndex = padIndex + bank * 16;
        const auto note = program->getNoteFromPad(padWithBankIndex);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);

        for (auto &p : padsWithSameNote)
        {
            const auto strip = mixerStrips[p - bank * 16];

            if (p >= bank * 16 && p < (bank + 1) * 16)
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
    const auto program = getProgramOrThrow();

    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());

    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto stereoMixer = getStereoMixerChannel(padIndex);
        const auto padIndexWithBank = padIndex + bank * 16;
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);

        for (auto &p : padsWithSameNote)
        {
            const auto strip = mixerStrips[p - bank * 16];

            if (p >= bank * 16 && p < (bank + 1) * 16)
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
    const auto program = getProgramOrThrow();

    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());

    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto stereoMixer = getStereoMixerChannel(padIndex);
        const auto indivFxMixer = getIndivFxMixerChannel(padIndex);

        const auto padIndexWithBank = padIndex + bank * 16;
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);

        for (auto &p : padsWithSameNote)
        {
            const auto stripIndex = p - bank * 16;
            const auto strip = mixerStrips[stripIndex];

            if (p >= bank * 16 && p < (bank + 1) * 16)
            {
                if (!stereoMixer)
                {
                    strip->setValueAString("");
                    continue;
                }

                if (stripHasStereoSound(stripIndex))
                {
                    strip->setValueAString(
                        stereoNames[indivFxMixer->getOutput()]);
                }
                else
                {
                    strip->setValueAString(
                        monoNames[indivFxMixer->getOutput()]);
                }
            }
        }
    }
}

void MixerScreen::displayIndivFaders()
{
    const auto program = getProgramOrThrow();

    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());

    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto indivFxMixer = getIndivFxMixerChannel(padIndex);
        const auto padIndexWithBank = padIndex + bank * 16;
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);

        for (auto &p : padsWithSameNote)
        {
            const auto strip = mixerStrips[p - bank * 16];

            if (p >= bank * 16 && p < (bank + 1) * 16)
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
    const auto program = getProgramOrThrow();

    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());

    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto indivFxMixer = getIndivFxMixerChannel(padIndex);
        const auto padIndexWithBank = padIndex + bank * 16;
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);

        for (auto &p : padsWithSameNote)
        {
            const auto strip = mixerStrips[p - bank * 16];

            if (p >= bank * 16 && p < (bank + 1) * 16)
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
    const auto program = getProgramOrThrow();

    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());

    for (int padIndex = 0; padIndex < 16; padIndex++)
    {
        if (!selection[padIndex])
        {
            continue;
        }

        const auto indivFxMixer = getIndivFxMixerChannel(padIndex);
        const auto padIndexWithBank = padIndex + bank * 16;
        const auto note = program->getNoteFromPad(padIndexWithBank);
        const auto padsWithSameNote = program->getPadIndicesFromNote(note);

        for (auto &p : padsWithSameNote)
        {
            const auto strip = mixerStrips[p - bank * 16];

            if (p >= bank * 16 && p < (bank + 1) * 16)
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

bool MixerScreen::stripHasStereoSound(int stripIndex) const
{
    const auto program = getProgramOrThrow();
    const int bank =
        static_cast<int>(mpc.clientEventController->getActiveBank());
    const auto pad = stripIndex + bank * 16;
    const auto note = program->getNoteFromPad(pad);
    const auto noteParameters = program->getNoteParameters(note);
    const auto soundIndex = noteParameters->getSoundIndex();
    return soundIndex != -1 && !sampler->getSound(soundIndex)->isMono();
}

void MixerScreen::pressPadIndexWithoutBank(const uint8_t padIndexWithoutBank)
{
    const bool shiftIsPressed =
        mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed();

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
