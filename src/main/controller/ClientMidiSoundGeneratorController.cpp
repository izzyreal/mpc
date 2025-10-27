#include "controller/ClientMidiSoundGeneratorController.hpp"

#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "audiomidi/EventHandler.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "controller/ClientEventController.hpp"
#include "sequencer/Track.hpp"

#include <iostream>
#include <optional>

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::audiomidi;
using namespace mpc::sequencer;

ClientMidiSoundGeneratorController::ClientMidiSoundGeneratorController(
    std::shared_ptr<ClientEventController> clientEventController,
    std::shared_ptr<MidiInputScreen> midiInputScreen,
    std::shared_ptr<EventHandler> eventHandler,
    std::shared_ptr<Sequencer> sequencer,
    std::shared_ptr<MultiRecordingSetupScreen> multiRecordingSetupScreen,
    std::shared_ptr<TimingCorrectScreen> timingCorrectScreen)
    : midiInputScreen(midiInputScreen), eventHandler(eventHandler),
      sequencer(sequencer),
      multiRecordingSetupScreen(multiRecordingSetupScreen),
      timingCorrectScreen(timingCorrectScreen),
      clientEventController(clientEventController)
{
    noteEventStore.reserve(NOTE_EVENT_STORE_CAPACITY);
}

void ClientMidiSoundGeneratorController::clearNoteEventStore()
{
    noteEventStore.clearPlayAndRecordStore();
}

void ClientMidiSoundGeneratorController::handleEvent(const ClientMidiEvent &e)
{
    using MessageType = ClientMidiEvent::MessageType;

    std::cout << "[SoundGenerator] Handling event type " << e.getMessageType()
              << " on channel " << e.getChannel() << std::endl;

    if (!sequencer->isRecordingModeMulti())
    {
        const int receiveCh = midiInputScreen->getReceiveCh();
        if (receiveCh != -1 && e.getChannel() != receiveCh)
        {
            return;
        }
    }

    if (!shouldProcessEvent(e))
    {
        return;
    }

    const auto type = e.getMessageType();

    if (type == MessageType::NOTE_ON)
    {
        handleNoteOnEvent(e);
    }
    else if (type == MessageType::NOTE_OFF)
    {
        handleNoteOffEvent(e);
    }
    else if (type == MessageType::CONTROLLER && e.getControllerNumber() == 7)
    {
        // TODO: handle CC7 / note variation slider
    }
    else if (type == MessageType::CHANNEL_PRESSURE)
    {
        // TODO: handle channel pressure
    }
    else if (type == MessageType::AFTERTOUCH)
    {
        // TODO: poly aftertouch
    }
    else if (type == MessageType::PROGRAM_CHANGE)
    {
        // TODO: program change
    }
    else if (type == MessageType::CONTROLLER && e.getControllerNumber() == 123)
    {
        // TODO: all notes off
    }
}

std::optional<int> ClientMidiSoundGeneratorController::getTrackIndexForEvent(
    const ClientMidiEvent &e) const
{
    if (sequencer->isRecordingModeMulti())
    {
        auto mrsLines = multiRecordingSetupScreen->getMrsLines();
        if (e.getChannel() < static_cast<int>(mrsLines.size()))
        {
            return mrsLines[e.getChannel()]->getTrack();
        }
        return std::nullopt;
    }
    return sequencer->getActiveTrackIndex();
}

std::shared_ptr<mpc::sequencer::Track>
ClientMidiSoundGeneratorController::getTrackForIndex(int trackIndex) const
{
    auto seq = sequencer->isPlaying() ? sequencer->getCurrentlyPlayingSequence()
                                      : sequencer->getActiveSequence();
    return seq->getTrack(trackIndex);
}

bool ClientMidiSoundGeneratorController::shouldProcessEvent(
    const ClientMidiEvent &e) const
{
    using MessageType = ClientMidiEvent::MessageType;

    if (!midiInputScreen->isMidiFilterEnabled())
    {
        return true;
    }

    const auto type = e.getMessageType();
    switch (type)
    {
        case MessageType::NOTE_ON:
        case MessageType::NOTE_OFF:
            return midiInputScreen->isNotePassEnabled();
        case MessageType::PITCH_WHEEL:
            return midiInputScreen->isPitchBendPassEnabled();
        case MessageType::PROGRAM_CHANGE:
            return midiInputScreen->isPgmChangePassEnabled();
        case MessageType::CHANNEL_PRESSURE:
            return midiInputScreen->isChPressurePassEnabled();
        case MessageType::AFTERTOUCH:
            return midiInputScreen->isPolyPressurePassEnabled();
        case MessageType::CONTROLLER:
        {
            const int ccNumber = e.getControllerNumber();
            const auto &ccPassEnabled = midiInputScreen->getCcPassEnabled();
            if (ccNumber < 0 ||
                ccNumber >= static_cast<int>(ccPassEnabled.size()))
            {
                return false;
            }
            return ccPassEnabled[ccNumber];
        }
        default:
            return true;
    }
}

void ClientMidiSoundGeneratorController::handleNoteOnEvent(
    const ClientMidiEvent &e)
{
    const int note = e.getNoteNumber();
    if (note < 35 || note > 98)
    {
        return;
    }

    auto trackIndexOpt = getTrackIndexForEvent(e);

    if (!trackIndexOpt)
    {
        return;
    }
    int trackIndex = *trackIndexOpt;

    auto noteOnEvent =
        std::make_shared<NoteOnEventPlayOnly>(note, e.getVelocity());

    auto track = getTrackForIndex(trackIndex);
    const int trackDevice = track->getDeviceIndex();
    const int trackVelocityRatio = track->getVelocityRatio();
    const std::optional<int> drumIndex =
        track->getBus() > 0 ? std::optional<int>(track->getBus() - 1)
                            : std::nullopt;

    eventHandler->handleMidiInputNoteOn(noteOnEvent, e.getBufferOffset(),
                                        trackIndex, trackDevice,
                                        trackVelocityRatio, drumIndex);

    noteEventStore.storePlayNoteEvent(
        std::pair<int, int>(trackIndex, noteOnEvent->getNote()), noteOnEvent);

    auto recordMidiNoteOn = std::make_shared<NoteOnEvent>(
        noteOnEvent->getNote(), noteOnEvent->getVelocity());

    auto mode = clientEventController->determineRecordingMode();

    switch (mode)
    {
        case RecordingMode::Overdub:
        {
            recordMidiNoteOn = track->recordNoteEventASync(
                noteOnEvent->getNote(), noteOnEvent->getVelocity());
            break;
        }
        case RecordingMode::Step:
        {
            recordMidiNoteOn = track->recordNoteEventSynced(
                sequencer->getTickPosition(), noteOnEvent->getNote(),
                noteOnEvent->getVelocity());
            sequencer->playMetronomeTrack();
            break;
        }
        case RecordingMode::RecMainWithoutPlaying:
        {
            recordMidiNoteOn = track->recordNoteEventSynced(
                sequencer->getTickPosition(), noteOnEvent->getNote(),
                noteOnEvent->getVelocity());
            sequencer->playMetronomeTrack();

            int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();
            if (stepLength != 1)
            {
                int bar = sequencer->getCurrentBarIndex() + 1;
                track->timingCorrect(0, bar, recordMidiNoteOn, stepLength,
                                     timingCorrectScreen->getSwing());

                if (recordMidiNoteOn->getTick() != sequencer->getTickPosition())
                {
                    sequencer->move(Sequencer::ticksToQuarterNotes(
                        recordMidiNoteOn->getTick()));
                }
            }
            break;
        }
        case RecordingMode::None:
        default:
            break;
    }

    if (recordMidiNoteOn)
    {
        noteEventStore.storeRecordNoteEvent(
            std::pair<int, int>(trackIndex, recordMidiNoteOn->getNote()),
            recordMidiNoteOn);
    }
}

void ClientMidiSoundGeneratorController::handleNoteOffEvent(
    const ClientMidiEvent &e)
{
    const int note = e.getNoteNumber();

    auto trackIndexOpt = getTrackIndexForEvent(e);
    if (!trackIndexOpt)
    {
        return;
    }
    int trackIndex = *trackIndexOpt;

    auto track = getTrackForIndex(trackIndex);
    const int trackDevice = track->getDeviceIndex();
    const std::optional<int> drumIndex =
        track->getBus() > 0 ? std::optional<int>(track->getBus() - 1)
                            : std::nullopt;

    // finalize recorded note if exists
    if (auto storedRecordMidiNoteOn = noteEventStore.retrieveRecordNoteEvent(
            std::pair<int, int>(trackIndex, note)))
    {
        auto mode = clientEventController->determineRecordingMode();

        switch (mode)
        {
            case RecordingMode::Overdub:
            {
                track->finalizeNoteEventASync(storedRecordMidiNoteOn);
                break;
            }
            case RecordingMode::Step:
            {
                int newDuration =
                    static_cast<int>(sequencer->getTickPosition());
                sequencer->stopMetronomeTrack();
                track->finalizeNoteEventSynced(storedRecordMidiNoteOn,
                                               newDuration);
                break;
            }
            case RecordingMode::RecMainWithoutPlaying:
            {
                int newDuration =
                    static_cast<int>(sequencer->getTickPosition());
                sequencer->stopMetronomeTrack();
                bool durationHasBeenAdjusted = track->finalizeNoteEventSynced(
                    storedRecordMidiNoteOn, newDuration);

                if (durationHasBeenAdjusted)
                {
                    int stepLength =
                        timingCorrectScreen->getNoteValueLengthInTicks();
                    int nextPos = sequencer->getTickPosition() + stepLength;
                    int bar = sequencer->getCurrentBarIndex() + 1;
                    nextPos = track->timingCorrectTick(
                        0, bar, nextPos, stepLength,
                        timingCorrectScreen->getSwing());
                    auto lastTick =
                        sequencer->getActiveSequence()->getLastTick();

                    if (nextPos != 0 && nextPos < lastTick)
                    {
                        sequencer->move(
                            Sequencer::ticksToQuarterNotes(nextPos));
                    }
                    else
                    {
                        sequencer->move(
                            Sequencer::ticksToQuarterNotes(lastTick));
                    }
                }
                break;
            }
            case RecordingMode::None:
            default:
                break;
        }
    }

    if (auto storedNoteOnEvent = noteEventStore.retrievePlayNoteEvent(
            std::pair<int, int>(trackIndex, note)))
    {
        eventHandler->handleMidiInputNoteOff(storedNoteOnEvent->getNoteOff(),
                                             e.getBufferOffset(), trackIndex,
                                             trackDevice, drumIndex);
    }
}
