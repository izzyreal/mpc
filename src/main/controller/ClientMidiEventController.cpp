#include "controller/ClientMidiEventController.hpp"
#include "sequencer/Transport.hpp"
#include "command/TriggerLocalNoteOffCommand.hpp"
#include "command/TriggerLocalNoteOnCommand.hpp"
#include "controller/ClientEventController.hpp"

#include "command/context/TriggerLocalNoteContextFactory.hpp"
#include "performance/EventTypes.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include <memory>

using namespace mpc::performance;
using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::lcdgui;
using namespace mpc::hardware;
using namespace mpc::engine;
using namespace mpc::command::context;

ClientMidiEventController::ClientMidiEventController(
    const std::weak_ptr<PerformanceManager> &performanceManager,
    const std::shared_ptr<ClientEventController> &clientEventController,
    std::shared_ptr<ClientHardwareEventController>
        clientHardwareEventController,
    std::shared_ptr<MidiSwScreen> midiSwScreen,
    std::weak_ptr<Sequencer> sequencer, const std::shared_ptr<Sampler> &sampler,
    const std::shared_ptr<MidiInputScreen> &midiInputScreen,
    const std::shared_ptr<audiomidi::EventHandler> &eventHandler,
    const std::shared_ptr<MultiRecordingSetupScreen> &multiRecordingSetupScreen,
    const std::shared_ptr<TimingCorrectScreen> &timingCorrectScreen,
    const std::shared_ptr<LayeredScreen> &layeredScreen,
    const std::shared_ptr<Hardware> &hardware,
    const std::shared_ptr<Screens> &screens,
    const std::shared_ptr<SequencerPlaybackEngine> &sequencerPlaybackEngine,
    PreviewSoundPlayer *previewSoundPlayer)
    : clientEventController(clientEventController),
      clientHardwareEventController(clientHardwareEventController),
      performanceManager(performanceManager), midiInputScreen(midiInputScreen),
      eventHandler(eventHandler), sequencer(sequencer), sampler(sampler),
      multiRecordingSetupScreen(multiRecordingSetupScreen),
      timingCorrectScreen(timingCorrectScreen), layeredScreen(layeredScreen),
      hardware(hardware), screens(screens),
      sequencerPlaybackEngine(sequencerPlaybackEngine),
      previewSoundPlayer(previewSoundPlayer)
{
    footswitchController =
        std::make_shared<ClientMidiFootswitchAssignmentController>(
            clientHardwareEventController, midiSwScreen, sequencer);
}

void ClientMidiEventController::handleClientMidiEvent(const ClientMidiEvent &e)
{
    if (clientEventController->getLayeredScreen()->isCurrentScreen(
            {ScreenId::MidiInputMonitorScreen}))
    {
        const auto notificationMessage =
            std::string("a") + std::to_string(e.getChannel());
        notifyObservers(notificationMessage);
    }

    if (!sequencer.lock()->isRecordingModeMulti())
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

    switch (e.getMessageType())
    {
        case MessageType::NOTE_ON:
            handleNoteOn(e);
            break;
        case MessageType::NOTE_OFF:
            handleNoteOff(e);
            break;
        case MessageType::AFTERTOUCH:
            handleKeyAftertouch(e);
            break;
        case MessageType::CHANNEL_PRESSURE:
            handleChannelAftertouch(e);
            break;
        case MessageType::PITCH_WHEEL:
            handlePitchBend(e);
            break;

        case MessageType::PROGRAM_CHANGE:
            handleProgramChange(e);
            break;

        case MessageType::CONTROLLER:
            handleControlChange(e);
            footswitchController->handleEvent(e);
            break;

        case MessageType::MIDI_CLOCK:
            handleMidiClock(e);
            syncController.handleEvent(e);
            break;
        case MessageType::MIDI_START:
            handleStart(e);
            syncController.handleEvent(e);
            break;
        case MessageType::MIDI_STOP:
            handleStop(e);
            syncController.handleEvent(e);
            break;
        case MessageType::MIDI_CONTINUE:
            handleContinue(e);
            syncController.handleEvent(e);
            break;

        default:
            break;
    }
}

std::shared_ptr<ClientMidiFootswitchAssignmentController>
ClientMidiEventController::getFootswitchAssignmentController()
{
    return footswitchController;
}

bool ClientMidiEventController::isOmniOn() const noexcept
{
    // return mode == 1 || mode == 2;
    return true;
}

bool ClientMidiEventController::isPolyMode() const noexcept
{
    // return mode == 1 || mode == 3;
    return true;
}

void ClientMidiEventController::handleNoteOn(const ClientMidiEvent &e)
{
    if (e.getVelocity() == 0)
    {
        handleNoteOff(e);
        return;
    }

    if (!isPolyMode())
    {
        enforceMonoMode(e.getChannel(), e.getNoteNumber());
    }

    if (convertSustainPedalToDuration && sustainPedalState[e.getChannel()])
    {
        holdNoteForSustain(e.getChannel(), e.getNoteNumber());
    }

    noteOnInternal(e.getChannel(), e.getNoteNumber(), e.getVelocity());

    auto track = getTrackForEvent(e);

    auto program = getProgramForEvent(e);

    ProgramPadIndex programPadIndex;

    auto screen = layeredScreen->getCurrentScreen();
    const auto screenId = layeredScreen->getCurrentScreenId();

    auto programIndex = screen->getProgramIndex();

    if (program->isUsed() && programIndex)
    {
        programPadIndex =
            program->getPadIndexFromNote(DrumNoteNumber(e.getNoteNumber()));

        if (programPadIndex != NoProgramIndex)
        {
            performanceManager.lock()->registerProgramPadPress(
                PerformanceEventSource::MidiInput,
                std::optional<MidiChannel>(e.getChannel()), screenId,
                track->getIndex(), screen->getBus()->busType,
                ProgramPadIndex(programPadIndex), Velocity(e.getVelocity()),
                ProgramIndex(*programIndex), NoPhysicalPadIndex);
        }
    }

    const std::function action =
        [noteNumber = e.getNoteNumber(), velocity = e.getVelocity(), track,
         screen, programPadIndex, program, this](void *userData)
    {
        const performance::NoteOnEvent *registryNoteOnEvent =
            static_cast<performance::NoteOnEvent *>(userData);

        const auto ctx =
            TriggerLocalNoteContextFactory::buildTriggerLocalNoteOnContext(
                PerformanceEventSource::MidiInput, *registryNoteOnEvent,
                NoteNumber(noteNumber), Velocity(velocity), track.get(),
                screen->getBus(), screen, programPadIndex, program, sequencer,
                sequencerPlaybackEngine, performanceManager.lock(),
                clientEventController, eventHandler, screens, hardware);

        command::TriggerLocalNoteOnCommand(ctx).execute();
    };

    performanceManager.lock()->registerNoteOn(
        PerformanceEventSource::MidiInput,
        std::optional<MidiChannel>(e.getChannel()), screenId, track->getIndex(),
        screen->getBus()->busType, NoteNumber(e.getNoteNumber()),
        Velocity(e.getVelocity()), std::optional(programIndex), action);
}

void ClientMidiEventController::handleNoteOff(const ClientMidiEvent &e)
{
    const int midiChannel = e.getChannel();
    const int noteNumber = e.getNoteNumber();

    if (convertSustainPedalToDuration && sustainPedalState[midiChannel])
    {
        holdNoteForSustain(midiChannel, noteNumber);
        return;
    }

    noteOffInternal(midiChannel, noteNumber);

    const std::function action = [this, noteNumber](void *userData)
    {
        const auto noteEventInfo =
            static_cast<performance::NoteOnEvent *>(userData);

        if (!noteEventInfo)
        {
            // TODO What do we do with orphaned note offs?
            return;
        }

        ProgramPadIndex programPadIndex = NoProgramPadIndex;
        std::shared_ptr<Program> program;

        const auto programIndex = noteEventInfo->programIndex;
        const auto trackIndex = noteEventInfo->trackIndex;

        if (programIndex != NoProgramIndex)
        {
            program = sampler->getProgram(programIndex);
        }

        if (program->isUsed())
        {
            programPadIndex =
                program->getPadIndexFromNote(DrumNoteNumber(noteNumber));

            if (programPadIndex != NoProgramPadIndex)
            {
                performanceManager.lock()->registerProgramPadRelease(
                    PerformanceEventSource::MidiInput,
                    ProgramPadIndex(programPadIndex), programIndex,
                    [](void *) {});
            }
        }

        const auto screen = screens->getScreenById(noteEventInfo->screenId);

        const auto ctx =
            TriggerLocalNoteContextFactory::buildTriggerLocalNoteOffContext(
                PerformanceEventSource::MidiInput, NoteNumber(noteNumber),
                noteEventInfo->recordNoteEventId,
                sequencer.lock()
                    ->getSelectedSequence()
                    ->getTrack(trackIndex)
                    .get(),
                noteEventInfo->busType, screen, programPadIndex, program,
                sequencer, sequencerPlaybackEngine, performanceManager.lock(),
                clientEventController, eventHandler, screens, hardware);

        command::TriggerLocalNoteOffCommand(ctx).execute();
    };

    performanceManager.lock()->registerNoteOff(
        PerformanceEventSource::MidiInput, NoteNumber(noteNumber),
        std::optional<MidiChannel>(midiChannel), action);
}

void ClientMidiEventController::handleKeyAftertouch(
    const ClientMidiEvent &e) const
{
    const auto pressure = e.getAftertouchValue();
    const auto note = e.getAftertouchNote();
    const auto track = getTrackForEvent(e);
    const auto bus = sequencer.lock()->getBus(track->getBusType());
    performanceManager.lock()->registerNoteAftertouch(
        PerformanceEventSource::MidiInput, NoteNumber(note), Pressure(pressure),
        std::optional<MidiChannel>(e.getChannel()));

    if (const auto program = getProgramForEvent(e); program->isUsed())
    {
        if (const auto programPadIndex =
                program->getPadIndexFromNote(DrumNoteNumber(note));
            programPadIndex != -1)
        {
            performanceManager.lock()->registerProgramPadAftertouch(
                PerformanceEventSource::MidiInput, programPadIndex,
                getProgramIndexForEvent(e), Pressure(pressure));
        }
    }
}

void ClientMidiEventController::handleChannelAftertouch(
    const ClientMidiEvent &e) const
{
    const auto pressure = e.getChannelPressure();
    const auto track = getTrackForEvent(e);
    const auto bus = sequencer.lock()->getBus(track->getBusType());

    for (auto &p : sampler->getPrograms())
    {
        if (const auto program = p.lock(); program)
        {
            auto programIndex = getProgramIndexForEvent(e);
            for (int programPadIndex = 0;
                 programPadIndex < Mpc2000XlSpecs::PROGRAM_PAD_COUNT;
                 ++programPadIndex)
            {
                performanceManager.lock()->registerProgramPadAftertouch(
                    PerformanceEventSource::MidiInput,
                    ProgramPadIndex(programPadIndex), programIndex,
                    Pressure(pressure));

                if (const auto note = program->getNoteFromPad(
                        ProgramPadIndex(programPadIndex));
                    note != NoDrumNoteAssigned)
                {
                    performanceManager.lock()->registerNoteAftertouch(
                        PerformanceEventSource::MidiInput, NoteNumber(note),
                        Pressure(pressure),
                        std::optional<MidiChannel>(e.getChannel()));
                }
            }
        }
    }
}

void ClientMidiEventController::handlePitchBend(const ClientMidiEvent &e) const
{
    // Pitch bend recognized (value range: -8192..8191)
}

void ClientMidiEventController::handleControlChange(const ClientMidiEvent &e)
{
    const int ch = e.getChannel();
    const int cc = e.getControllerNumber();
    const int val = e.getControllerValue();

    // Recognized controllers 0–127
    if (cc == 64) // Sustain Pedal
    {
        if (convertSustainPedalToDuration)
        {
            sustainPedalState[ch] = val >= 64;
            if (!sustainPedalState[ch])
            {
                releaseSustainedNotes(ch);
            }
        }
        else
        {
            // Record sustain pedal message as-is
        }
    }
    else if (cc == 123)
    {
        // Should we do something else here? Trigger drum note offs for example?
        performanceManager.lock()->clear();
    }

    const auto program = getProgramForEvent(e);
    const auto sliderControllerNumber =
        program->getSlider()->getControlChange();

    if (cc == sliderControllerNumber && sliderControllerNumber >= 1)
    {
        // Verify on real 2KXL: what is the 0-based and 1-based range for this
        // property of the program?
        ClientHardwareEvent event;
        event.source = ClientHardwareEvent::Source::Internal;
        event.componentId = SLIDER;
        event.type = ClientHardwareEvent::Type::SliderMove;
        event.value = 1.f - e.getControllerValue() / 127.f;
        clientEventController->handleClientEvent(ClientEvent{event});
    }
}

void ClientMidiEventController::handleProgramChange(
    const ClientMidiEvent &e) const
{
    if (midiInputScreen->getProgChangeSeq())
    {
        if (e.getProgramNumber() >= 0 &&
            e.getProgramNumber() <= Mpc2000XlSpecs::LAST_SEQUENCE_INDEX)
        {
            if (sequencer.lock()->getTransport()->isPlaying())
            {
                if (!sequencer.lock()
                         ->getTransport()
                         ->isRecordingOrOverdubbing() &&
                    sequencer.lock()
                        ->getSequence(e.getProgramNumber())
                        ->isUsed())
                {
                    if (clientEventController->getLayeredScreen()
                            ->isCurrentScreen({ScreenId::NextSeqScreen,
                                               ScreenId::NextSeqPadScreen,
                                               ScreenId::SequencerScreen}))
                    {
                        sequencer.lock()->setNextSq(
                            SequenceIndex(e.getProgramNumber()));
                    }
                }
            }
            else
            {
                sequencer.lock()->setSelectedTrackIndex(e.getProgramNumber());
            }
        }
    }
    else
    {
        if (const auto drumBus = getDrumBusForEvent(e); drumBus)
        {
            if (sampler->getProgram(e.getProgramNumber())->isUsed())
            {
                drumBus->setProgramIndex(ProgramIndex(e.getProgramNumber()));
            }
        }
    }
}

void ClientMidiEventController::handleSystemExclusive(
    const ClientMidiEvent &e) const
{
    // SysEx recognized only if ID == 0x45
}

void ClientMidiEventController::handleSongPositionPointer(
    const ClientMidiEvent &e) const
{
    // Recognized System Common message
}

void ClientMidiEventController::handleSongSelect(const ClientMidiEvent &e) const
{
    // Ignored by MPC2000XL
}

void ClientMidiEventController::handleTuneRequest(
    const ClientMidiEvent &e) const
{
    // Ignored by MPC2000XL
}

void ClientMidiEventController::handleMidiClock(const ClientMidiEvent &e) const
{
    // Recognized Real-Time message
}

void ClientMidiEventController::handleStart(const ClientMidiEvent &e) const
{
    // Recognized Real-Time message
}

void ClientMidiEventController::handleStop(const ClientMidiEvent &e) const
{
    // Recognized Real-Time message
}

void ClientMidiEventController::handleContinue(const ClientMidiEvent &e) const
{
    // Recognized Real-Time message
}

void ClientMidiEventController::handleLocalOnOff(const ClientMidiEvent &e) const
{
    // Not recognized
}

void ClientMidiEventController::handleAllNotesOff(
    const ClientMidiEvent &e) const
{
    // Not recognized
}

void ClientMidiEventController::handleActiveSense(
    const ClientMidiEvent &e) const
{
    // Not recognized
}

void ClientMidiEventController::handleReset(const ClientMidiEvent &e) const
{
    // Not recognized
}

void ClientMidiEventController::noteOnInternal(int channel, int note,
                                               int velocity)
{
    heldNotes[channel].push_back(note);
}

void ClientMidiEventController::noteOffInternal(int channel, int note)
{
    auto &notes = heldNotes[channel];
    notes.erase(std::remove(notes.begin(), notes.end(), note), notes.end());
}

void ClientMidiEventController::holdNoteForSustain(int channel, int note)
{
    sustainedNotes[channel].push_back(note);
}

void ClientMidiEventController::releaseSustainedNotes(int channel)
{
    for (const int note : sustainedNotes[channel])
    {
        noteOffInternal(channel, note);
    }
    sustainedNotes[channel].clear();
}

void ClientMidiEventController::enforceMonoMode(int channel, int newNote)
{
    if (!heldNotes[channel].empty())
    {
        for (const int note : heldNotes[channel])
        {
            noteOffInternal(channel, note);
        }
    }
}

std::optional<int>
ClientMidiEventController::getTrackIndexForEvent(const ClientMidiEvent &e) const
{
    if (sequencer.lock()->isRecordingModeMulti())
    {
        if (const auto mrsLines = multiRecordingSetupScreen->getMrsLines();
            e.getChannel() < static_cast<int>(mrsLines.size()))
        {
            return mrsLines[e.getChannel()]->getTrack();
        }
        return std::nullopt;
    }
    return sequencer.lock()->getSelectedTrackIndex();
}

std::shared_ptr<mpc::sequencer::Track>
ClientMidiEventController::getTrackForEvent(const ClientMidiEvent &e) const
{
    if (const auto trackIndex = getTrackIndexForEvent(e); trackIndex)
    {
        const auto seq = sequencer.lock()->getTransport()->isPlaying()
                             ? sequencer.lock()->getCurrentlyPlayingSequence()
                             : sequencer.lock()->getSelectedSequence();
        return seq->getTrack(*trackIndex);
    }

    return {};
}

std::shared_ptr<DrumBus>
ClientMidiEventController::getDrumBusForEvent(const ClientMidiEvent &e) const
{
    if (const auto drumBusType = getDrumBusTypeForEvent(e); drumBusType)
    {
        return sequencer.lock()->getDrumBus(*drumBusType);
    }
    return {};
}

std::shared_ptr<mpc::sampler::Program>
ClientMidiEventController::getProgramForEvent(const ClientMidiEvent &e) const
{
    if (const auto drumBusType = getDrumBusTypeForEvent(e); drumBusType)
    {
        return sampler->getProgram(
            sequencer.lock()->getDrumBus(*drumBusType)->getProgramIndex());
    }

    return {};
}

mpc::ProgramIndex ClientMidiEventController::getProgramIndexForEvent(
    const ClientMidiEvent &e) const
{
    if (const auto drumBusType = getDrumBusTypeForEvent(e); drumBusType)
    {
        return sequencer.lock()->getDrumBus(*drumBusType)->getProgramIndex();
    }

    return NoProgramIndex;
}

std::optional<BusType> ClientMidiEventController::getDrumBusTypeForEvent(
    const ClientMidiEvent &e) const
{
    const auto track = getTrackForEvent(e);
    if (!track)
    {
        return std::nullopt;
    }

    return isDrumBusType(track->getBusType())
               ? std::optional(track->getBusType())
               : std::nullopt;
}

bool ClientMidiEventController::shouldProcessEvent(
    const ClientMidiEvent &e) const
{
    if (!midiInputScreen->isMidiFilterEnabled())
    {
        return true;
    }

    switch (e.getMessageType())
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
