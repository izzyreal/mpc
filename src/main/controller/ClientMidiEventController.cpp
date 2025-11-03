#include "controller/ClientMidiEventController.hpp"
#include "command/TriggerLocalNoteOffCommand.hpp"
#include "command/TriggerLocalNoteOnCommand.hpp"
#include "controller/ClientEventController.hpp"

#include "command/context/TriggerLocalNoteContextFactory.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"

#include "client/event/ClientHardwareEvent.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include <memory>

using namespace mpc::eventregistry;
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
    std::shared_ptr<EventRegistry> eventRegistry,
    std::shared_ptr<ClientEventController> clientEventController,
    std::shared_ptr<ClientHardwareEventController>
        clientHardwareEventController,
    std::shared_ptr<MidiSwScreen> midiSwScreen,
    std::shared_ptr<Sequencer> sequencer, std::shared_ptr<Sampler> sampler,
    std::shared_ptr<MidiInputScreen> midiInputScreen,
    std::shared_ptr<audiomidi::EventHandler> eventHandler,
    std::shared_ptr<MultiRecordingSetupScreen> multiRecordingSetupScreen,
    std::shared_ptr<TimingCorrectScreen> timingCorrectScreen,
    std::shared_ptr<LayeredScreen> layeredScreen,
    std::shared_ptr<Hardware> hardware, std::shared_ptr<Screens> screens,
    std::shared_ptr<FrameSeq> frameSequencer,
    PreviewSoundPlayer *previewSoundPlayer)
    : clientEventController(clientEventController),
      clientHardwareEventController(clientHardwareEventController),
      eventRegistry(eventRegistry), midiInputScreen(midiInputScreen),
      eventHandler(eventHandler), sequencer(sequencer), sampler(sampler),
      multiRecordingSetupScreen(multiRecordingSetupScreen),
      timingCorrectScreen(timingCorrectScreen), layeredScreen(layeredScreen),
      hardware(hardware), screens(screens), frameSequencer(frameSequencer),
      previewSoundPlayer(previewSoundPlayer)
{
    footswitchController =
        std::make_shared<ClientMidiFootswitchAssignmentController>(
            clientHardwareEventController, midiSwScreen, sequencer);
}

void ClientMidiEventController::handleClientMidiEvent(const ClientMidiEvent &e)
{
    if (clientEventController->getLayeredScreen()
            ->isCurrentScreen<ScreenId::MidiInputMonitorScreen>())
    {
        const auto notificationMessage =
            std::string("a") + std::to_string(e.getChannel());
        notifyObservers(notificationMessage);
    }

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
    const int ch = e.getChannel();
    const int note = e.getNoteNumber();
    const int vel = e.getVelocity();

    if (vel == 0)
    {
        handleNoteOff(e);
        return;
    }

    if (!isPolyMode())
    {
        enforceMonoMode(ch, note);
    }

    if (convertSustainPedalToDuration && sustainPedalState[ch])
    {
        holdNoteForSustain(ch, note);
    }

    noteOnInternal(ch, note, vel);

    if (note < 35 || note > 98)
    {
        return;
    }

    auto track = getTrackForEvent(e);

    if (!track)
    {
        return;
    }

    auto program = getProgramForEvent(e);

    if (!program)
    {
        return;
    }

    const int programPadIndex = program->getPadIndexFromNote(note);

    if (programPadIndex == -1)
    {
        return;
    }

    auto ctx = TriggerLocalNoteContextFactory::buildTriggerDrumNoteOnContext(
        eventregistry::Source::MidiInput, layeredScreen, clientEventController,
        hardware, sequencer, screens, sampler, eventRegistry, eventHandler,
        frameSequencer, previewSoundPlayer, programPadIndex, e.getVelocity(),
        layeredScreen->getCurrentScreen());

    command::TriggerLocalNoteOnCommand(ctx).execute();
}

void ClientMidiEventController::handleNoteOff(const ClientMidiEvent &e)
{
    const int ch = e.getChannel();
    const int note = e.getNoteNumber();

    if (convertSustainPedalToDuration && sustainPedalState[ch])
    {
        holdNoteForSustain(ch, note);
        return;
    }

    noteOffInternal(ch, note);

    auto track = getTrackForEvent(e);

    if (!track)
    {
        return;
    }

    int trackIndex = track->getIndex();

    auto program = getProgramForEvent(e);

    if (!program)
    {
        return;
    }

    const int programPadIndex = program->getPadIndexFromNote(note);

    if (programPadIndex == -1)
    {
        return;
    }

    auto snapshot = eventRegistry->getSnapshot();

    auto noteEventInfo =
        snapshot.retrieveNoteEvent(note, eventregistry::Source::MidiInput);

    if (!noteEventInfo)
    {
        // printf("no noteEventInfo found!\n");
        return;
    }

    auto drumBus = std::dynamic_pointer_cast<DrumBus>(noteEventInfo->bus);

    if (!drumBus)
    {
        return;
    }

    auto ctx = TriggerLocalNoteContextFactory::buildTriggerDrumNoteOffContext(
        eventregistry::Source::MidiInput, previewSoundPlayer, eventRegistry,
        eventHandler, screens, sequencer, hardware, clientEventController,
        frameSequencer, programPadIndex, drumBus->getIndex(),
        noteEventInfo->screen, note, noteEventInfo->program,
        noteEventInfo->track);

    command::TriggerLocalNoteOffCommand(ctx).execute();
}

void ClientMidiEventController::handleKeyAftertouch(const ClientMidiEvent &e)
{
    auto pressure = e.getAftertouchValue();
    auto note = e.getAftertouchNote();
    auto track = getTrackForEvent(e);
    auto bus = sequencer->getBus<Bus>(track->getBus());
    auto registrySnapshot = eventRegistry->getSnapshot();
    eventRegistry->registerNoteAftertouch(eventregistry::Source::MidiInput,
                                          note, pressure, e.getChannel());

    if (auto program = getProgramForEvent(e); program)
    {
        if (auto programPadIndex = program->getPadIndexFromNote(note);
            programPadIndex != -1)
        {
            eventRegistry->registerProgramPadAftertouch(
                eventregistry::Source::MidiInput, bus, program, programPadIndex,
                pressure, track.get());
        }
    }
}

void ClientMidiEventController::handleChannelAftertouch(
    const ClientMidiEvent &e)
{
    auto pressure = e.getChannelPressure();
    auto track = getTrackForEvent(e);
    auto bus = sequencer->getBus<Bus>(track->getBus());

    for (auto &p : sampler->getPrograms())
    {
        if (auto program = p.lock(); program)
        {
            for (int programPadIndex = 0; programPadIndex < 64;
                 ++programPadIndex)
            {
                eventRegistry->registerProgramPadAftertouch(
                    eventregistry::Source::MidiInput, bus, program,
                    programPadIndex, pressure, track.get());

                if (auto note = program->getNoteFromPad(programPadIndex);
                    note != -1)
                {
                    eventRegistry->registerNoteAftertouch(
                        eventregistry::Source::MidiInput, note, pressure,
                        e.getChannel());
                }
            }
        }
    }
}

void ClientMidiEventController::handlePitchBend(const ClientMidiEvent &e)
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
            sustainPedalState[ch] = (val >= 64);
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
        eventRegistry->clear();
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
        event.componentId = hardware::ComponentId::SLIDER;
        event.type = ClientHardwareEvent::Type::SliderMove;
        event.value = 1.f - (e.getControllerValue() / 127.f);
        clientEventController->handleClientEvent(ClientEvent{event});
    }
}

void ClientMidiEventController::handleProgramChange(const ClientMidiEvent &e)
{
    if (midiInputScreen->getProgChangeSeq())
    {
        if (e.getProgramNumber() >= 0 &&
            e.getProgramNumber() <= Mpc2000XlSpecs::LAST_SEQUENCE_INDEX)
        {
            if (sequencer->isPlaying())
            {
                if (!sequencer->isRecordingOrOverdubbing() &&
                    sequencer->getSequence(e.getProgramNumber())->isUsed())
                {
                    if (clientEventController->getLayeredScreen()
                            ->isCurrentScreen<ScreenId::NextSeqScreen,
                                              ScreenId::NextSeqPadScreen,
                                              ScreenId::SequencerScreen>())
                    {
                        sequencer->setNextSq(e.getProgramNumber());
                    }
                }
            }
            else
            {
                sequencer->setActiveTrackIndex(e.getProgramNumber());
            }
        }
    }
    else
    {
        if (auto drumBus = getDrumBusForEvent(e); drumBus)
        {
            if (sampler->getProgram(e.getProgramNumber()))
            {
                drumBus->setProgram(e.getProgramNumber());
            }
        }
    }
}

void ClientMidiEventController::handleSystemExclusive(const ClientMidiEvent &e)
{
    // SysEx recognized only if ID == 0x45
}

void ClientMidiEventController::handleSongPositionPointer(
    const ClientMidiEvent &e)
{
    // Recognized System Common message
}

void ClientMidiEventController::handleSongSelect(const ClientMidiEvent &e)
{
    // Ignored by MPC2000XL
}

void ClientMidiEventController::handleTuneRequest(const ClientMidiEvent &e)
{
    // Ignored by MPC2000XL
}

void ClientMidiEventController::handleMidiClock(const ClientMidiEvent &e)
{
    // Recognized Real-Time message
}

void ClientMidiEventController::handleStart(const ClientMidiEvent &e)
{
    // Recognized Real-Time message
}

void ClientMidiEventController::handleStop(const ClientMidiEvent &e)
{
    // Recognized Real-Time message
}

void ClientMidiEventController::handleContinue(const ClientMidiEvent &e)
{
    // Recognized Real-Time message
}

void ClientMidiEventController::handleLocalOnOff(const ClientMidiEvent &e)
{
    // Not recognized
}

void ClientMidiEventController::handleAllNotesOff(const ClientMidiEvent &e)
{
    // Not recognized
}

void ClientMidiEventController::handleActiveSense(const ClientMidiEvent &e)
{
    // Not recognized
}

void ClientMidiEventController::handleReset(const ClientMidiEvent &e)
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
    for (int note : sustainedNotes[channel])
    {
        noteOffInternal(channel, note);
    }
    sustainedNotes[channel].clear();
}

void ClientMidiEventController::enforceMonoMode(int channel, int newNote)
{
    if (!heldNotes[channel].empty())
    {
        for (int note : heldNotes[channel])
        {
            noteOffInternal(channel, note);
        }
    }
}

std::optional<int>
ClientMidiEventController::getTrackIndexForEvent(const ClientMidiEvent &e) const
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

std::shared_ptr<Track>
ClientMidiEventController::getTrackForEvent(const ClientMidiEvent &e) const
{
    if (auto trackIndex = getTrackIndexForEvent(e); trackIndex)
    {
        auto seq = sequencer->isPlaying()
                       ? sequencer->getCurrentlyPlayingSequence()
                       : sequencer->getActiveSequence();
        return seq->getTrack(*trackIndex);
    }

    return {};
}

std::shared_ptr<DrumBus>
ClientMidiEventController::getDrumBusForEvent(const ClientMidiEvent &e) const
{
    if (auto drumIndex = getDrumIndexForEvent(e); drumIndex)
    {
        return sequencer->getDrumBus(*drumIndex);
    }
    return {};
}

std::shared_ptr<Program>
ClientMidiEventController::getProgramForEvent(const ClientMidiEvent &e) const
{
    if (auto drumIndex = getDrumIndexForEvent(e); drumIndex)
    {
        return sampler->getProgram(
            sequencer->getDrumBus(*drumIndex)->getProgram());
    }

    return {};
}

std::optional<int>
ClientMidiEventController::getDrumIndexForEvent(const ClientMidiEvent &e) const
{
    auto track = getTrackForEvent(e);
    if (!track)
    {
        return std::nullopt;
    }

    return track->getBus() > 0 ? std::optional<int>(track->getBus() - 1)
                               : std::nullopt;
}

bool ClientMidiEventController::shouldProcessEvent(
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
