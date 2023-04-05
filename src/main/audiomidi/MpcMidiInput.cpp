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
#include <controls/GlobalReleaseControls.hpp>

#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>
#include <lcdgui/screens/MidiSwScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>

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
          eventAdapter(std::make_unique<EventAdapter>(mpc, sequencer)),
          midiFullControl(std::make_unique<VmpcMidiControlMode>())
{
}

std::string MpcMidiInput::getName()
{
    return "mpcmidiin" + std::to_string(index);
}

void MpcMidiInput::transport(MidiMessage *msg, int timeStamp)
{
    auto shortMsg = dynamic_cast<ShortMessage *>(msg);

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");

    if (vmpcSettingsScreen->midiControlMode == VmpcSettingsScreen::MidiControlMode::VMPC)
    {
        midiFullControl->processMidiInputEvent(mpc, shortMsg);
        return;
    }

    eventAdapter->process(msg);

    auto status = msg->getStatus();
    std::string notificationMessage = std::string(index == 0 ? "a" : "b");

    auto channel = shortMsg->getChannel();

    notificationMessage += std::to_string(channel);

    notifyObservers(notificationMessage);

    auto isControl = status >= ShortMessage::CONTROL_CHANGE && status < ShortMessage::CONTROL_CHANGE + 16;

    if (isControl)
    {
        handleControl(shortMsg);
    }
    else
    {
        handlePolyAndNote(msg);
    }

    auto event = eventAdapter->get().lock();

    if (!event)
    {
        return;
    }

    auto mce = std::dynamic_pointer_cast<mpc::sequencer::MidiClockEvent>(event);
    //auto note = std::dynamic_pointer_cast<mpc::sequencer::NoteEvent>(event);
    auto noteOn = std::dynamic_pointer_cast<mpc::sequencer::NoteOnEvent>(event);
    auto noteOff = std::dynamic_pointer_cast<mpc::sequencer::NoteOffEvent>(event);

    auto syncScreen = mpc.screens->get<SyncScreen>("sync");

    std::shared_ptr<mpc::sequencer::Track> track;

    if (mce && syncScreen->in == index)
    {
        if (syncScreen->getModeIn() != 0)
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
    else if (noteOn)
    {
        noteOn->setTick(-1);
        auto s = sequencer->isPlaying() ? sequencer->getCurrentlyPlayingSequence()
            : sequencer->getActiveSequence();
        track = s->getTrack(noteOn->getTrack());

        int pad = -1;

        if (track->getBus() > 0)
        {
            auto p = sampler->getProgram(sampler->getDrumBusProgramIndex(track->getBus()));
            Util::setSliderNoteVariationParameters(mpc, noteOn, p);
            pad = p->getPadIndexFromNote(noteOn->getNote());
        }

        if (track->getBus() > 0 && track->getIndex() < 64 &&
            mpc.getControls()->isTapPressed() && sequencer->isPlaying())
        {
            return;
        }

        if (pad == -1)
        {
            mpc.getEventHandler()->handleNoThru(noteOn, track.get(), timeStamp);

            if (sequencer->isRecordingOrOverdubbing())
            {
                //noteOn->setDuration(note->getVelocity() == 0 ? 0 : -1);
                noteOn->setTick(sequencer->getTickPosition());

                auto recordedEvent = track->recordNoteEventASync(noteOn->getNote(), noteOn->getVelocity());
                noteOn->CopyValuesTo(recordedEvent);
            }
        }
        else
        {
            mpc.getActiveControls()->pad(pad, noteOn->getVelocity());
        }
    }    
    else if (noteOff)
    {
        noteOff->setTick(-1);
        auto s = sequencer->isPlaying() ? sequencer->getCurrentlyPlayingSequence()
            : sequencer->getActiveSequence();
        track = s->getTrack(noteOff->getTrack());

        int pad = -1;

        if (track->getBus() > 0)
        {
            auto p = sampler->getProgram(sampler->getDrumBusProgramIndex(track->getBus()));
            //Util::setSliderNoteVariationParameters(mpc, noteOff, p);
            pad = p->getPadIndexFromNote(noteOff->getNote());
        }

        if (pad == -1)
        {
            mpc.getEventHandler()->handleNoThru(noteOff, track.get(), timeStamp);

            if (sequencer->isRecordingOrOverdubbing())
            {
                noteOff->setTick(sequencer->getTickPosition());
                
                track->finalizeNoteEventASync(noteOff->getNote());
            }
        }
        else
        {
            mpc.getReleaseControls()->simplePad(pad);
        }
    }
    if (noteOn || noteOff)
    {
        auto midiOutputScreen = mpc.screens->get<MidiOutputScreen>("midi-output");

        switch (midiOutputScreen->getSoftThru())
        {
        case 0:
            break;
        case 1:
            midiOut(eventAdapter->get(), track.get());
            break;
        case 2:
            transportOmni(msg, "a");
            break;
        case 3:
            transportOmni(msg, "b");
            break;
        case 4:
            transportOmni(msg, "a");
            transportOmni(msg, "b");
            break;
        }
    }
    /*else if (note)
    {
        note->setTick(-1);
        auto s = sequencer->isPlaying() ? sequencer->getCurrentlyPlayingSequence()
                                         : sequencer->getActiveSequence();
        auto track = s->getTrack(note->getTrack());

        int pad = -1;

        if (track->getBus() > 0)
        {
            auto p = sampler->getProgram(sampler->getDrumBusProgramIndex(track->getBus()));
            Util::setSliderNoteVariationParameters(mpc, note, p);
            pad = p->getPadIndexFromNote(note->getNote());
        }

        if (note->getVelocity() != 0 && track->getBus() > 0 && track->getIndex() < 64 &&
            mpc.getControls()->isTapPressed() && sequencer->isPlaying())
        {
            return;
        }

        if (pad == -1)
        {
            mpc.getEventHandler()->handleNoThru(note, track.get(), timeStamp);

            if (sequencer->isRecordingOrOverdubbing())
            {
                note->setDuration(note->getVelocity() == 0 ? 0 : -1);
                note->setTick(sequencer->getTickPosition());

                if (note->getVelocity() == 0)
                {
                    track->recordNoteOffNow(note->getNote());
                }
                else
                {
                    auto recordedEvent = track->recordNoteOnNow(note->getNote());
                    note->CopyValuesTo(recordedEvent);
                }
            }
        }
        else
        {
            if (note->getVelocity() == 0)
            {
                mpc.getReleaseControls()->simplePad(pad);
            }
            else
            {
                auto activeControls = mpc.getActiveControls();
                activeControls->pad(pad, note->getVelocity());
            }
        }

        auto midiOutputScreen = mpc.screens->get<MidiOutputScreen>("midi-output");

        switch (midiOutputScreen->getSoftThru())
        {
            case 0:
                break;
            case 1:
                midiOut(eventAdapter->get(), track.get());
                break;
            case 2:
                transportOmni(msg, "a");
                break;
            case 3:
                transportOmni(msg, "b");
                break;
            case 4:
                transportOmni(msg, "a");
                transportOmni(msg, "b");
                break;
        }
    }*/
}

void MpcMidiInput::handleControl(ShortMessage *shortMsg)
{
    const auto controller = shortMsg->getData1();
    const auto value = shortMsg->getData2();

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
        midiInputScreen->getReceiveCh() == shortMsg->getChannel())
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

void MpcMidiInput::handlePolyAndNote(MidiMessage *msg)
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
            auto channelPressureValue = (*msg->getMessage())[1];

            // Alternatively we could process these like note offs,
            // but this seems better.
            if (channelPressureValue > 0)
            {
                for (auto &p: mpc.getHardware()->getPads())
                {
                    if (p->isPressed())
                    {
                        p->setPressure(channelPressureValue);
                    }
                }
            }
        }
    }
}
