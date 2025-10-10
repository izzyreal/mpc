#include "MidiInput.hpp"
#include "controls/PadPressScreenUpdateContext.h"
#include "controller/PadContextFactory.h"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/EventHandler.hpp>
#include <audiomidi/MidiOutput.hpp>
#include <audiomidi/VmpcMidiControlMode.hpp>
#include <controls/BaseControls.hpp>
#include <controls/GlobalReleaseControls.hpp>
#include "hardware2/Hardware2.h"
#include "hardware2/HardwareComponent.h"
#include <hardware/Hardware.hpp>
#include <hardware/TopPanel.hpp>

#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>
#include <lcdgui/screens/MidiSwScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>
#include <lcdgui/screens/window/MultiRecordingSetupScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/StepEditOptionsScreen.hpp>

#include <sequencer/MidiClockEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

#include <engine/midi/MidiMessage.hpp>
#include <engine/midi/ShortMessage.hpp>

using namespace mpc::audiomidi;
using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::engine::midi;

MidiInput::MidiInput(mpc::Mpc &_mpc, int _index)
        : mpc(_mpc),
          sequencer(_mpc.getSequencer()),
          sampler(_mpc.getSampler()),
          index(_index),
          midiFullControl(std::make_unique<VmpcMidiControlMode>())
{
}

void MidiInput::transport(MidiMessage *midiMsg, int timeStamp)
{
    const auto msg = dynamic_cast<ShortMessage*>(midiMsg);

    if (mpc.getLayeredScreen()->getCurrentScreenName() == "midi-input-monitor")
    {
        const auto notificationMessage = std::string(index == 0 ? "a" : "b") + std::to_string(msg->getChannel());
        notifyObservers(notificationMessage);
    }

    const auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");

    if (vmpcSettingsScreen->midiControlMode == VmpcSettingsScreen::MidiControlMode::VMPC)
    {
        midiFullControl->processMidiInputEvent(mpc, msg);
        return;
    }

    const auto midiInputScreen = mpc.screens->get<MidiInputScreen>("midi-input");

    if (midiInputScreen->getReceiveCh() != -1 && msg->getChannel() != midiInputScreen->getReceiveCh())
    {
        return;
    }
    else if (msg->isMidiClock())
    {
        handleMidiClock(msg);
    }
    else if (msg->isNoteOn() || msg->isNoteOff())
    {
        if (msg->isNoteOn())
        {
            handleNoteOn(msg, timeStamp);
        }
        else if (msg->isNoteOff())
        {
            handleNoteOff(msg, timeStamp);
        }

        switch (mpc.screens->get<MidiOutputScreen>("midi-output")->getSoftThru())
        {
            case 1:
                // Soft thru:OFF
                break;
            case 2:
                transportOmni(midiMsg, "a");
                break;
            case 3:
                transportOmni(midiMsg, "b");
                break;
            case 4:
                transportOmni(midiMsg, "a");
                transportOmni(midiMsg, "b");
                break;
        }
    }
    else if (msg->isControlChange())
    {
        handleControlChange(msg);
    }
    else if (msg->isChannelPressure())
    {
        handleChannelPressure(msg);
    }
}

void MidiInput::handleControlChange(ShortMessage* msg)
{
    const auto controller = msg->getData1();
    const auto value = msg->getData2();

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
    auto hardware = mpc.getHardware();
    auto hardware2 = mpc.getHardware2();

    // As per MPC2000XL's MIDI implementation chart
    if (controller == 7)
    {
        // It looks like the internal implementation of the slider is inverted.
        // At least, the Akai MPD16 sends value 0 at the bottom of the slider
        // and 127 at the top.
        // For now, we're safe to simply invert the input, but it would be nice
        // to make this congruent with the MPD16 (assuming it's the same for
        // other controllers, but it would be nice to verify some).
        hardware2->getSlider()->setValue(127 - value);
    }

    auto midiInputScreen = mpc.screens->get<MidiInputScreen>("midi-input");
    auto midiSwScreen = mpc.screens->get<MidiSwScreen>("midi-sw");

    if (midiInputScreen->getReceiveCh() == -1 ||
        midiInputScreen->getReceiveCh() == msg->getChannel())
    {
        std::vector<int> processedControllers;

        for (int i = 0; i < MidiSwScreen::SWITCH_COUNT; i++)
        {
            auto ctrl = midiSwScreen->getSwitch(i).first;

            if (ctrl == 0)
            {
                continue;
            }

            ctrl -= 1;

            if (find(begin(processedControllers), end(processedControllers), ctrl) != end(processedControllers))
            {
                continue;
            }

            processedControllers.push_back(ctrl);

            auto func = midiSwScreen->getSwitch(i).second;

            if (ctrl == controller)
            {
                if (value >= 64)
                {
                    if (func == 0)
                    {
                        sequencer->playFromStart();
                    }
                    else if (func == 1)
                    {
                        sequencer->play();
                    }
                    else if (func == 2)
                    {
                        sequencer->stop();
                    }
                    else if (func == 3)
                    {
                        sequencer->rec();
                    }
                    else if (func == 4)
                    {
                        sequencer->overdub();
                    }
                    else if (func == 5)
                    {
                        if (sequencer->isRecording())
                        {
                            sequencer->setRecording(false);
                        }
                        else if (sequencer->isPlaying())
                        {
                            sequencer->setRecording(true);
                        }
                        else
                        {
                            sequencer->play();
                        }
                    }
                    else if (func == 6)
                    {
                        if (sequencer->isOverDubbing())
                        {
                            sequencer->setOverdubbing(false);
                        }
                        else if (sequencer->isPlaying())
                        {
                            sequencer->setOverdubbing(true);
                        }
                        else
                        {
                            sequencer->play();
                        }
                    }
                    else if (func == 7)
                    {
                        auto activeControls = mpc.getActiveControls();
                        activeControls->tap();
                    }
                    else if (func >= 8 && func < 12)
                    {
                        auto bank = func - 8;
                        mpc.setBank(bank);
                    }
                    else if (func >= 12 && func < 28)
                    {
                        auto pad = func - 12;
                        hardware2->getPad(pad)->pressWithVelocity(value);
                    }
                }
                else // value < 64
                {
                    if (func == 7)
                    {
                        mpc.getReleaseControls()->tap();
                    }
                    else if (func >= 12 && func < 28)
                    {
                        auto pad = func - 12;
                        hardware2->getPad(pad)->release();
                    }
                }
            }
        }
    }
}

void MidiInput::midiOut(Track* track)
{
    auto deviceNumber = track->getDeviceIndex() - 1;

    std::string notificationLetter = "a";

    if (deviceNumber > 15)
    {
        deviceNumber -= 16;
        notificationLetter = "b";
    }

    if (mpc.getLayeredScreen()->getCurrentScreenName() == "midi-output-monitor")
    {
        notifyObservers(notificationLetter + std::to_string(deviceNumber));
    }
}

void MidiInput::transportOmni(MidiMessage *msg, const std::string& outputLetter)
{
    auto mpcMidiOutput = mpc.getMidiOutput();
    auto screenName = mpc.getLayeredScreen()->getCurrentScreenName();

    if (dynamic_cast<ShortMessage*>(msg) != nullptr && screenName == "midi-output-monitor")
    {
        notifyObservers(std::string(outputLetter + std::to_string(dynamic_cast<ShortMessage*>(msg)->getChannel())));
    }
}

void MidiInput::handleNoteOn(ShortMessage* msg, const int& timeStamp)
{
    auto playMidiNoteOn = std::make_shared<NoteOnEventPlayOnly>(msg);
    int trackNumber;
    if (sequencer->isRecordingModeMulti())
    {
        auto mrs = mpc.screens->get<MultiRecordingSetupScreen>("multi-recording-setup")->getMrsLines();
        trackNumber = mrs[msg->getChannel()]->getTrack();
    }
    else
    {
        trackNumber = sequencer->getActiveTrackIndex();
    }
    auto s = sequencer->isPlaying() ? sequencer->getCurrentlyPlayingSequence()
        : sequencer->getActiveSequence();
    auto track = s->getTrack(trackNumber);

    int padIndexWithBank = -1;
    auto bus = track->getBus();

    if (bus > 0)
    {
        padIndexWithBank = sampler->getProgram(sampler->getDrumBusProgramIndex(bus))->getPadIndexFromNote(playMidiNoteOn->getNote());
        if (track->getIndex() < 64 && mpc.getControls()->isTapPressed() && sequencer->isPlaying())
        {
            return;
        }
    }

    if (padIndexWithBank != -1)
    {
        auto ctx = controller::PadContextFactory::buildPadPushContext(mpc, padIndexWithBank, playMidiNoteOn->getVelocity(), mpc.getLayeredScreen()->getCurrentScreenName());
        mpc::controls::BaseControls::pad(ctx, padIndexWithBank, playMidiNoteOn->getVelocity());
        return;
    }
    else
    {
        const bool allowCentralNoteAndPadUpdate = std::find(
                mpc::controls::BaseControls::allowCentralNoteAndPadUpdateScreens.begin(),
                mpc::controls::BaseControls::allowCentralNoteAndPadUpdateScreens.end(),
                mpc.getLayeredScreen()->getCurrentScreenName()) != mpc::controls::BaseControls::allowCentralNoteAndPadUpdateScreens.end();

        std::function<void(int)> setMpcNote = [mpc = &mpc] (int n) { mpc->setNote(n); };
        std::function<void(int)> setMpcPad = [mpc = &mpc] (int p) { mpc->setPad(p); };

        controls::PadPressScreenUpdateContext padPressScreenUpdateContext {
            mpc.getLayeredScreen()->getCurrentScreenName(),
            mpc.getHardware()->getTopPanel()->isSixteenLevelsEnabled(),
            mpc::sequencer::isDrumNote(playMidiNoteOn->getNote()),
            allowCentralNoteAndPadUpdate,
            mpc.getActiveControls(),
            setMpcNote,
            setMpcPad,
            mpc.getLayeredScreen()->getFocus(),
            mpc.getBank()
        };

        controls::BaseControls::padPressScreenUpdate(padPressScreenUpdateContext, playMidiNoteOn->getNote());
        mpc.getEventHandler()->handleNoThru(playMidiNoteOn, track.get(), timeStamp);
        storePlayNoteEvent(std::pair<int, int>(trackNumber, playMidiNoteOn->getNote()), playMidiNoteOn);
        
        auto recordMidiNoteOn = std::make_shared<NoteOnEvent>(msg);
        if (sequencer->isRecordingOrOverdubbing())
        {
            recordMidiNoteOn = track->recordNoteEventASync(playMidiNoteOn->getNote(), playMidiNoteOn->getVelocity());
        }
        else if (mpc.getControls()->isStepRecording())
        {
            recordMidiNoteOn = track->recordNoteEventSynced(sequencer->getTickPosition(), playMidiNoteOn->getNote(), playMidiNoteOn->getVelocity());
            sequencer->playMetronomeTrack();
        }
        else if (mpc.getControls()->isRecMainWithoutPlaying())
        {
            recordMidiNoteOn = track->recordNoteEventSynced(sequencer->getTickPosition(), playMidiNoteOn->getNote(), playMidiNoteOn->getVelocity());
            sequencer->playMetronomeTrack();
            auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
            int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();

            if (stepLength != 1)
            {
                int bar = sequencer->getCurrentBarIndex() + 1;
                track->timingCorrect(0, bar, recordMidiNoteOn, stepLength, timingCorrectScreen->getSwing());

                if (recordMidiNoteOn->getTick() != sequencer->getTickPosition())
                    sequencer->move(Sequencer::ticksToQuarterNotes(recordMidiNoteOn->getTick()));
            }
        }

        if (recordMidiNoteOn)
        {
            storeRecordNoteEvent(std::pair<int, int>(trackNumber, recordMidiNoteOn->getNote()), recordMidiNoteOn);
        }
    }
}

void MidiInput::handleNoteOff(ShortMessage* msg, const int& timeStamp)
{
    int note = msg->getData1();
    int trackNumber;
    if (sequencer->isRecordingModeMulti())
    {
        auto mrs = mpc.screens->get<MultiRecordingSetupScreen>("multi-recording-setup")->getMrsLines();
        trackNumber = mrs[msg->getChannel()]->getTrack();
    }
    else
    {
        trackNumber = sequencer->getActiveTrackIndex();
    }
    
    auto s = sequencer->isPlaying() ? sequencer->getCurrentlyPlayingSequence()
        : sequencer->getActiveSequence();
    auto track = s->getTrack(trackNumber);

    int padIndexWithBank = -1;

    if (track->getBus() > 0)
    {
        padIndexWithBank = sampler->getProgram(sampler->getDrumBusProgramIndex(track->getBus()))->getPadIndexFromNote(msg->getData1());
    }

    if (padIndexWithBank != -1)
    {
        const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
        auto ctx = controller::PadContextFactory::buildPadReleaseContext(mpc, padIndexWithBank, currentScreenName);
        controls::GlobalReleaseControls::simplePad(ctx);
        return;
    }
    else if (auto storedRecordMidiNoteOn = retrieveRecordNoteEvent(std::pair<int, int>(trackNumber, note)))
    {
        auto step = mpc.getControls()->isStepRecording();
        auto recWithoutPlaying = mpc.getControls()->isRecMainWithoutPlaying();
        if (sequencer->isRecordingOrOverdubbing())
        {
            track->finalizeNoteEventASync(storedRecordMidiNoteOn);
        }
        else if (step || recWithoutPlaying)
        {
            auto newDuration = static_cast<int>(sequencer->getTickPosition());

            const auto stepEditOptionsScreen = mpc.screens->get<StepEditOptionsScreen>("step-edit-options");
            const auto increment = stepEditOptionsScreen->isAutoStepIncrementEnabled();
            const auto durationIsTcValue = stepEditOptionsScreen->isDurationOfRecordedNotesTcValue();
            const auto tcValuePercentage = stepEditOptionsScreen->getTcValuePercentage();

            const auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");

            const int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();

            if (step && durationIsTcValue)
            {
                newDuration = static_cast<int>(stepLength * (tcValuePercentage * 0.01));

                if (newDuration < 1)
                {
                    newDuration = 1;
                }
            }

            sequencer->stopMetronomeTrack();
            bool durationHasBeenAdjusted = track->finalizeNoteEventSynced(storedRecordMidiNoteOn, newDuration);

            if ((durationHasBeenAdjusted && recWithoutPlaying) || (step && increment))
            {
                int nextPos = sequencer->getTickPosition() + stepLength;
                auto bar = sequencer->getCurrentBarIndex() + 1;
                nextPos = track->timingCorrectTick(0, bar, nextPos, stepLength, timingCorrectScreen->getSwing());
                auto lastTick = sequencer->getActiveSequence()->getLastTick();

                if (nextPos != 0 && nextPos < lastTick)
                {
                    sequencer->move(Sequencer::ticksToQuarterNotes(nextPos));
                }
                else
                {
                    sequencer->move(Sequencer::ticksToQuarterNotes(lastTick));
                }
            }
        }  
    }

    if (auto storedmidiNoteOn = retrievePlayNoteEvent(std::pair<int, int>(trackNumber, note)))
    {
        mpc.getEventHandler()->handleNoThru(storedmidiNoteOn->getNoteOff(), track.get(), timeStamp);
        return;
    }
}

void MidiInput::handleMidiClock(ShortMessage* msg)
{
    auto mce = std::make_shared<MidiClockEvent>(msg->getStatus());
    auto syncScreen = mpc.screens->get<SyncScreen>("sync");
    
    if (syncScreen->in == index && syncScreen->getModeIn() != 0)
    {
        switch (mce->getStatus())
        {
        case ShortMessage::START:
            sequencer->playFromStart();
            break;
        case ShortMessage::CONTINUE:
            sequencer->play();
            break;
        case ShortMessage::STOP:
            sequencer->stop();
            break;
        case ShortMessage::TIMING_CLOCK:
            break;
        }
    }
}

void MidiInput::handleChannelPressure(ShortMessage* msg)
{
    auto s = sequencer->getActiveSequence();
    auto channelPressureValue = (*msg->getMessage())[1];

    if (channelPressureValue > 0)
    {
        for (auto& p: mpc.getHardware2()->getPads())
        {
        if (p->isPressed())
        {
            p->aftertouch(channelPressureValue);
        }
        }
    }
}
