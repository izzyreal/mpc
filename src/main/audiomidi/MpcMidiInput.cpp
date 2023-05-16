#include "MpcMidiInput.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/DataWheel.hpp>
#include <hardware/HwPad.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/Button.hpp>
#include <hardware/Pot.hpp>
#include <audiomidi/EventHandler.hpp>
#include <audiomidi/MpcMidiOutput.hpp>
#include <audiomidi/VmpcMidiControlMode.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <controls/GlobalReleaseControls.hpp>


#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>
#include <lcdgui/screens/MidiSwScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>
#include <lcdgui/screens/window/MultiRecordingSetupScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/StepEditOptionsScreen.hpp>

#include <sequencer/Event.hpp>
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

MpcMidiInput::MpcMidiInput(mpc::Mpc &_mpc, int _index)
        : mpc(_mpc),
          sequencer(_mpc.getSequencer()),
          sampler(_mpc.getSampler()),
          index(_index),
          midiFullControl(std::make_unique<VmpcMidiControlMode>())
{
}

void MpcMidiInput::transport(MidiMessage *midiMsg, int timeStamp)
{
    auto msg = dynamic_cast<ShortMessage*>(midiMsg);

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
    auto midiInputScreen = mpc.screens->get<MidiInputScreen>("midi-input");

    if (vmpcSettingsScreen->midiControlMode == VmpcSettingsScreen::MidiControlMode::VMPC)
    {
        midiFullControl->processMidiInputEvent(mpc, msg);
        return;
    }
    std::shared_ptr<mpc::sequencer::Event> event;
    if (midiInputScreen->getReceiveCh() != -1 && msg->getChannel() != midiInputScreen->getReceiveCh())
    {
        return;
    }
    else if (msg->isMidiClock())
    {
        event = handleMidiClock(msg);
    }
    else if (msg->isNoteOn() || msg->isNoteOff())
    {
        if (msg->isNoteOn())
        {
            event = handleNoteOn(msg, timeStamp);
        }
        else if (msg->isNoteOff())
        {
            event = handleNoteOff(msg, timeStamp);
        }

        switch (mpc.screens->get<MidiOutputScreen>("midi-output")->getSoftThru())
        {
            case 0:
                break;
            case 1:
                //midiOut(event, track.get());
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

    if (event)//?????????
    {
        std::string notificationMessage = std::string(index == 0 ? "a" : "b") + std::to_string(msg->getChannel());
        notifyObservers(notificationMessage);
    }
}

void MpcMidiInput::handleControlChange(mpc::engine::midi::ShortMessage* msg)
{
    const auto controller = msg->getData1();
    const auto value = msg->getData2();

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
    auto hardware = mpc.getHardware();

    // As per MPC2000XL's MIDI implementation chart
    if (controller == 7)
    {
        // It looks like the internal implementation of the slider is inverted.
        // At least, the Akai MPD16 sends value 0 at the bottom of the slider
        // and 127 at the top.
        // For now, we're safe to simply invert the input, but it would be nice
        // to make this congruent with the MPD16 (assuming it's the same for
        // other controllers, but it would be nice to verify some).
        hardware->getSlider()->setValue(127 - value);
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
                        hardware->getPad(pad)->push(value);
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
                        hardware->getPad(pad)->release();
                    }
                }
            }
        }
    }
}

void MpcMidiInput::midiOut(std::weak_ptr<mpc::sequencer::Event> e, mpc::sequencer::Track *track)
{
    auto event = e.lock();
    std::string notificationLetter;
    auto deviceNumber = track->getDeviceIndex() - 1;

    if (deviceNumber != -1 && deviceNumber < 32)
    {
        auto channel = deviceNumber;

        if (channel > 15)
        {
            channel -= 16;
        }

    }

    auto mpcMidiOutput = mpc.getMidiOutput();

    notificationLetter = "a";

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

void MpcMidiInput::transportOmni(MidiMessage *msg, std::string outputLetter)
{
    auto mpcMidiOutput = mpc.getMidiOutput();
    auto screenName = mpc.getLayeredScreen()->getCurrentScreenName();

    if (dynamic_cast<ShortMessage*>(msg) != nullptr && screenName == "midi-output-monitor")
    {
        notifyObservers(std::string(outputLetter + std::to_string(dynamic_cast<ShortMessage*>(msg)->getChannel())));
    }
}

std::shared_ptr<mpc::sequencer::NoteOnEvent> mpc::audiomidi::MpcMidiInput::handleNoteOn(mpc::engine::midi::ShortMessage* msg, const int& timeStamp)
{
    auto midiNoteOn = std::make_shared<NoteOnEventPlayOnly>(msg);
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

    int pad = -1;
    auto bus = track->getBus();
    if (bus > 0)
    {
        pad = sampler->getProgram(sampler->getDrumBusProgramIndex(bus))->getPadIndexFromNote(midiNoteOn->getNote());
        //Util::setSliderNoteVariationParameters(mpc, midiNoteOn, p);
    }

    if (bus > 0 && track->getIndex() < 64 &&
        mpc.getControls()->isTapPressed() && sequencer->isPlaying())
    {
        return nullptr;
    }

    if (pad != -1)
    {
        mpc.getActiveControls()->pad(pad, midiNoteOn->getVelocity());
        return nullptr;
    }
    else
    {
        if (!storeNoteEvent(std::pair<int, int>(trackNumber, midiNoteOn->getNote()),midiNoteOn)) return nullptr;
        if (sequencer->isRecordingOrOverdubbing())
        {
            track->recordNoteEventASync(midiNoteOn->getNote(), midiNoteOn->getVelocity());
        }
        else if (mpc.getControls()->isStepRecording())
        {
            track->recordNoteEventSynced(sequencer->getTickPosition(), midiNoteOn->getNote(), midiNoteOn->getVelocity());
        }
        else if (mpc.getControls()->isRecMainWithoutPlaying())
        {
            auto recordNoteOnEvent = track->recordNoteEventSynced(sequencer->getTickPosition(), midiNoteOn->getNote(), midiNoteOn->getVelocity());
            auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
            int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();

            if (stepLength != 1)
            {
                int bar = sequencer->getCurrentBarIndex() + 1;
                track->timingCorrect(0, bar, recordNoteOnEvent, stepLength, timingCorrectScreen->getSwing());

                if (recordNoteOnEvent->getTick() != sequencer->getTickPosition())
                    sequencer->move(recordNoteOnEvent->getTick());
            }
        }
        mpc.getEventHandler()->handleNoThru(midiNoteOn, track.get(), timeStamp);
    }
    return midiNoteOn;
}

std::shared_ptr<mpc::sequencer::NoteOffEvent> mpc::audiomidi::MpcMidiInput::handleNoteOff(mpc::engine::midi::ShortMessage* msg, const int& timeStamp)
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

    int pad = -1;

    if (track->getBus() > 0)
    {
        pad = sampler->getProgram(sampler->getDrumBusProgramIndex(track->getBus()))->getPadIndexFromNote(msg->getData1());
        //Util::setSliderNoteVariationParameters(mpc, noteOff, p);
    }

    if (pad != -1)
    {
        mpc.getReleaseControls()->simplePad(pad);
        return nullptr;
    }
    else if (auto storedmidiNoteOn = retrieveNoteEvent(std::pair<int, int>(trackNumber, note)); storedmidiNoteOn)
    {
        auto step = mpc.getControls()->isStepRecording();
        auto recWithoutPlaying = mpc.getControls()->isRecMainWithoutPlaying();
        if (sequencer->isRecordingOrOverdubbing())
        {
            track->finalizeNoteEventASync(note);
        }
        else if (step || recWithoutPlaying)
        {
            auto newDuration = static_cast<int>(mpc.getAudioMidiServices()->getFrameSequencer()->getTickPosition());

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
            bool durationHasBeenAdjusted = track->finalizeNoteEventSynced(note, newDuration);

            if ((durationHasBeenAdjusted && recWithoutPlaying) || (step && increment))
            {
                int nextPos = sequencer->getTickPosition() + stepLength;
                auto bar = sequencer->getCurrentBarIndex() + 1;
                nextPos = track->timingCorrectTick(0, bar, nextPos, stepLength, timingCorrectScreen->getSwing());
                auto lastTick = sequencer->getActiveSequence()->getLastTick();

                if (nextPos != 0 && nextPos < lastTick)
                {
                    sequencer->move(nextPos);
                }
                else
                {
                    sequencer->move(lastTick);
                }
            }
        }
        mpc.getEventHandler()->handleNoThru(storedmidiNoteOn->getNoteOff(), track.get(), timeStamp);
        return storedmidiNoteOn->getNoteOff();
    }
    return nullptr;
}

std::shared_ptr<mpc::sequencer::MidiClockEvent> mpc::audiomidi::MpcMidiInput::handleMidiClock(mpc::engine::midi::ShortMessage* msg)
{
    auto mce = std::make_shared<mpc::sequencer::MidiClockEvent>(msg->getStatus());
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
    return mce;
}

void MpcMidiInput::handleChannelPressure(mpc::engine::midi::ShortMessage* msg)
{
    auto s = sequencer->getActiveSequence();
    auto t = s->getTrack(sequencer->getActiveTrackIndex());

    auto bus = t->getBus();

    if (bus != 0)
    {
        auto status = msg->getStatus();
        auto isChannelPressure =
                status >= ShortMessage::CHANNEL_PRESSURE && status < ShortMessage::CHANNEL_PRESSURE + 16;
        if (isChannelPressure)
        {
            //auto channelPressureValue = (*msg->getMessage())[1];

            //// Alternatively we could process these like note offs,
            //// but this seems better.
            //if (channelPressureValue > 0)
            //{
            //    for (auto &p: mpc.getHardware()->getPads())
            //    {
            //        if (p->isPressed())
            //        {
            //            p->setPressure(channelPressureValue);
            //        }
            //    }
            //}
        }
    }
}
