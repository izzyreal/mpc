#include "controller/ClientMidiSoundGeneratorController.hpp"

#include "eventregistry/EventRegistry.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "audiomidi/EventHandler.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

#include "sampler/Sampler.hpp"
#include "sampler/Program.hpp"

#include "controller/ClientEventController.hpp"

#include "command/context/TriggerDrumContextFactory.hpp"

#include <iostream>
#include <optional>

using namespace mpc::eventregistry;
using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::audiomidi;
using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::command::context;

ClientMidiSoundGeneratorController::ClientMidiSoundGeneratorController(
    std::shared_ptr<EventRegistry> eventRegistry,
    std::shared_ptr<ClientEventController> clientEventController,
    std::shared_ptr<MidiInputScreen> midiInputScreen,
    std::shared_ptr<EventHandler> eventHandler,
    std::shared_ptr<Sequencer> sequencer, std::shared_ptr<Sampler> sampler,
    std::shared_ptr<MultiRecordingSetupScreen> multiRecordingSetupScreen,
    std::shared_ptr<TimingCorrectScreen> timingCorrectScreen,
    std::shared_ptr<LayeredScreen> layeredScreen,
    std::shared_ptr<hardware::Hardware> hardware,
    std::shared_ptr<lcdgui::Screens> screens,
    std::shared_ptr<sequencer::FrameSeq> frameSequencer,
    engine::PreviewSoundPlayer *previewSoundPlayer)
    : eventRegistry(eventRegistry), midiInputScreen(midiInputScreen),
      eventHandler(eventHandler), sequencer(sequencer), sampler(sampler),
      multiRecordingSetupScreen(multiRecordingSetupScreen),
      timingCorrectScreen(timingCorrectScreen),
      clientEventController(clientEventController),
      layeredScreen(layeredScreen), hardware(hardware), screens(screens),
      previewSoundPlayer(previewSoundPlayer), frameSequencer(frameSequencer)
{
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

    const auto program = getProgramForEvent(e);
    const auto sliderControllerNumber =
        program->getSlider()->getControlChange();

    if (type == MessageType::NOTE_ON)
    {
        handleNoteOnEvent(e);
    }
    else if (type == MessageType::NOTE_OFF)
    {
        handleNoteOffEvent(e);
    }
    else if (type == MessageType::CONTROLLER &&
             e.getControllerNumber() == sliderControllerNumber)
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
    else if (type == MessageType::CHANNEL_PRESSURE)
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
                            eventregistry::Source::MidiInput, note, pressure);
                    }
                }
            }
        }
    }
    else if (type == MessageType::AFTERTOUCH)
    {
        // TODO: poly aftertouch
        // Seems like the MPC2000XL does not implement poly aftertouch reception
        // as per the MIDI implementation char. Double check on the real 2KXL...
        auto pressure = e.getAftertouchValue();
        auto note = e.getAftertouchNote();
        auto track = getTrackForEvent(e);
        auto bus = sequencer->getBus<Bus>(track->getBus());
        auto registrySnapshot = eventRegistry->getSnapshot();
        eventRegistry->registerNoteAftertouch(eventregistry::Source::MidiInput,
                                              note, pressure);

        if (auto program = getProgramForEvent(e); program)
        {
            if (auto programPadIndex = program->getPadIndexFromNote(note);
                programPadIndex != -1)
            {
                eventRegistry->registerProgramPadAftertouch(
                    eventregistry::Source::MidiInput, bus, program,
                    programPadIndex, pressure, track.get());
            }
        }
    }
    else if (type == MessageType::PROGRAM_CHANGE)
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
                                ->isCurrentScreen<NextSeqScreen,
                                                  NextSeqPadScreen,
                                                  SequencerScreen>())
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
    else if (type == MessageType::CONTROLLER && e.getControllerNumber() == 123)
    {
        // TODO
        // Send early note offs
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

std::shared_ptr<Track> ClientMidiSoundGeneratorController::getTrackForEvent(
    const ClientMidiEvent &e) const
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

std::shared_ptr<DrumBus> ClientMidiSoundGeneratorController::getDrumBusForEvent(
    const ClientMidiEvent &e) const
{
    if (auto drumIndex = getDrumIndexForEvent(e); drumIndex)
    {
        return sequencer->getDrumBus(*drumIndex);
    }
    return {};
}

std::shared_ptr<Program> ClientMidiSoundGeneratorController::getProgramForEvent(
    const ClientMidiEvent &e) const
{
    if (auto drumIndex = getDrumIndexForEvent(e); drumIndex)
    {
        return sampler->getProgram(
            sequencer->getDrumBus(*drumIndex)->getProgram());
    }

    return {};
}

std::optional<int> ClientMidiSoundGeneratorController::getDrumIndexForEvent(
    const ClientMidiEvent &e) const
{
    auto track = getTrackForEvent(e);
    if (!track)
    {
        return std::nullopt;
    }

    return track->getBus() > 0 ? std::optional<int>(track->getBus() - 1)
                               : std::nullopt;
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

    auto ctx = TriggerDrumContextFactory::buildTriggerDrumNoteOnContext(
        eventregistry::Source::MidiInput, layeredScreen, clientEventController,
        hardware, sequencer, screens, sampler, eventRegistry, eventHandler,
        frameSequencer, previewSoundPlayer, programPadIndex, e.getVelocity(),
        layeredScreen->getCurrentScreen());

    command::TriggerDrumNoteOnCommand(ctx).execute();

    /*

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
        // noteEventStore.storeRecordNoteEvent(
        //     std::pair<int, int>(trackIndex, recordMidiNoteOn->getNote()),
        //     recordMidiNoteOn);
    }*/
}

void ClientMidiSoundGeneratorController::handleNoteOffEvent(
    const ClientMidiEvent &e)
{
    const int note = e.getNoteNumber();

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
        printf("no noteEventInfo found!\n");
        return;
    }

    auto drumBus = std::dynamic_pointer_cast<DrumBus>(noteEventInfo->bus);

    if (!drumBus)
    {
        return;
    }

    auto ctx = TriggerDrumContextFactory::buildTriggerDrumNoteOffContext(
        eventregistry::Source::MidiInput, previewSoundPlayer, eventRegistry,
        eventHandler, screens, sequencer, hardware, clientEventController,
        frameSequencer, programPadIndex, drumBus->getIndex(),
        noteEventInfo->screen, note, noteEventInfo->program,
        noteEventInfo->track);

    command::TriggerDrumNoteOffCommand(ctx).execute();

    // finalize recorded note if exists
    // if (auto storedRecordMidiNoteOn = noteEventStore.retrieveRecordNoteEvent(
    //         std::pair<int, int>(trackIndex, note)))
    // {
    //     auto mode = clientEventController->determineRecordingMode();
    //
    //     switch (mode)
    //     {
    //         case RecordingMode::Overdub:
    //         {
    //             track->finalizeNoteEventASync(storedRecordMidiNoteOn);
    //             break;
    //         }
    //         case RecordingMode::Step:
    //         {
    //             int newDuration =
    //                 static_cast<int>(sequencer->getTickPosition());
    //             sequencer->stopMetronomeTrack();
    //             track->finalizeNoteEventSynced(storedRecordMidiNoteOn,
    //                                            newDuration);
    //             break;
    //         }
    //         case RecordingMode::RecMainWithoutPlaying:
    //         {
    //             int newDuration =
    //                 static_cast<int>(sequencer->getTickPosition());
    //             sequencer->stopMetronomeTrack();
    //             bool durationHasBeenAdjusted =
    //             track->finalizeNoteEventSynced(
    //                 storedRecordMidiNoteOn, newDuration);
    //
    //             if (durationHasBeenAdjusted)
    //             {
    //                 int stepLength =
    //                     timingCorrectScreen->getNoteValueLengthInTicks();
    //                 int nextPos = sequencer->getTickPosition() + stepLength;
    //                 int bar = sequencer->getCurrentBarIndex() + 1;
    //                 nextPos = track->timingCorrectTick(
    //                     0, bar, nextPos, stepLength,
    //                     timingCorrectScreen->getSwing());
    //                 auto lastTick =
    //                     sequencer->getActiveSequence()->getLastTick();
    //
    //                 if (nextPos != 0 && nextPos < lastTick)
    //                 {
    //                     sequencer->move(
    //                         Sequencer::ticksToQuarterNotes(nextPos));
    //                 }
    //                 else
    //                 {
    //                     sequencer->move(
    //                         Sequencer::ticksToQuarterNotes(lastTick));
    //                 }
    //             }
    //             break;
    //         }
    //         case RecordingMode::None:
    //         default:
    //             break;
    //     }
    // }
    //
    // if (auto storedNoteOnEvent = noteEventStore.retrievePlayNoteEvent(
    //         std::pair<int, int>(trackIndex, note)))
    // {
    //     eventHandler->handleMidiInputNoteOff(storedNoteOnEvent->getNoteOff(),
    //                                          e.getBufferOffset(), trackIndex,
    //                                          trackDevice, drumIndex);
    // }
}
