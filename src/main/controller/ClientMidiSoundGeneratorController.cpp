#include "controller/ClientMidiSoundGeneratorController.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "audiomidi/EventHandler.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SeqUtil.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/ComponentId.hpp"

#include <iostream>

using namespace mpc::controller;
using namespace mpc::client::event;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::audiomidi;
using namespace mpc::sequencer;

ClientMidiSoundGeneratorController::ClientMidiSoundGeneratorController(
    std::shared_ptr<MidiInputScreen> midiInputScreen,
    std::shared_ptr<EventHandler> eventHandler,
    std::shared_ptr<Sequencer> sequencer,
    std::shared_ptr<MultiRecordingSetupScreen> multiRecordingSetupScreen,
    std::shared_ptr<TimingCorrectScreen> timingCorrectScreen,
    std::shared_ptr<ClientHardwareEventController> clientHardwareEventController,
    std::shared_ptr<mpc::hardware::Button> recButton,
    std::function<std::string()> getCurrentScreenNameToUse)
    : midiInputScreen(midiInputScreen), eventHandler(eventHandler), sequencer(sequencer), 
      multiRecordingSetupScreen(multiRecordingSetupScreen), timingCorrectScreen(timingCorrectScreen),
      clientHardwareEventController(clientHardwareEventController), recButton(recButton), 
      getCurrentScreenName(getCurrentScreenNameToUse)
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

    // Apply MidiInputScreen channel filtering when not in multi recording mode
    if (!sequencer->isRecordingModeMulti())
    {
        const int receiveCh = midiInputScreen->getReceiveCh();
        if (receiveCh != -1 && e.getChannel() != receiveCh)
        {
            return; // Skip processing if channel doesn't match receive channel
        }
    }

    const auto type = e.getMessageType();

    // Apply MidiInputScreen message type filters
    if (midiInputScreen->isMidiFilterEnabled())
    {
        switch (type)
        {
        case MessageType::NOTE_ON:
        case MessageType::NOTE_OFF:
            if (!midiInputScreen->isNotePassEnabled())
                return;
            break;
        case MessageType::PITCH_WHEEL:
            if (!midiInputScreen->isPitchBendPassEnabled())
                return;
            break;
        case MessageType::PROGRAM_CHANGE:
            if (!midiInputScreen->isPgmChangePassEnabled())
                return;
            break;
        case MessageType::CHANNEL_PRESSURE:
            if (!midiInputScreen->isChPressurePassEnabled())
                return;
            break;
        case MessageType::AFTERTOUCH:
            if (!midiInputScreen->isPolyPressurePassEnabled())
                return;
            break;
        case MessageType::CONTROLLER:
            {
                const int ccNumber = e.getControllerNumber();
                const auto& ccPassEnabled = midiInputScreen->getCcPassEnabled();
                if (ccNumber < ccPassEnabled.size() && !ccPassEnabled[ccNumber])
                    return;
            }
            break;
        default:
            break;
        }
    }

    const auto currentScreenName = getCurrentScreenName();

    if (type == MessageType::NOTE_ON)
    {
        const int note = e.getNoteNumber();

        // Check if note is within valid range (35-98)
        if (note >= 35 && note <= 98)
        {
            // Create a NoteOnEventPlayOnly directly from ClientMidiEvent data
            auto noteOnEvent = std::make_shared<NoteOnEventPlayOnly>(note, e.getVelocity());
            
            // Derive track information based on recording mode
            int trackIndex;
            if (sequencer->isRecordingModeMulti())
            {
                // In multi recording mode, use MultiRecordingSetupScreen to map MIDI channel to track
                auto mrsLines = multiRecordingSetupScreen->getMrsLines();
                if (e.getChannel() < mrsLines.size())
                {
                    trackIndex = mrsLines[e.getChannel()]->getTrack();
                }
                else
                {
                    // Channel out of range, skip processing
                    return;
                }
            }
            else
            {
                // In single recording mode, use active track
                trackIndex = sequencer->getActiveTrackIndex();
            }
            
            // Get track information
            auto sequence = sequencer->isPlaying() ? sequencer->getCurrentlyPlayingSequence() : sequencer->getActiveSequence();
            auto track = sequence->getTrack(trackIndex);
            
            const int trackDevice = track->getDeviceIndex();
            const int trackVelocityRatio = track->getVelocityRatio();
            const std::optional<int> drumIndex = track->getBus() > 0 ? std::optional<int>(track->getBus() - 1) : std::nullopt;
            
            // Call the EventHandler's handleMidiInputNoteOn method
            eventHandler->handleMidiInputNoteOn(noteOnEvent, e.getBufferOffset(), trackIndex, trackDevice, trackVelocityRatio, drumIndex);
            
            // Store the play note event for later retrieval on note off
            noteEventStore.storePlayNoteEvent(std::pair<int, int>(trackIndex, noteOnEvent->getNote()), noteOnEvent);
            
            // Handle recording note events
            auto recordMidiNoteOn = std::make_shared<NoteOnEvent>(noteOnEvent->getNote(), noteOnEvent->getVelocity());

            if (sequencer->isRecordingOrOverdubbing())
            {
                recordMidiNoteOn = track->recordNoteEventASync(noteOnEvent->getNote(), noteOnEvent->getVelocity());
            }
            else if (SeqUtil::isStepRecording(currentScreenName, sequencer))
            {
                recordMidiNoteOn = track->recordNoteEventSynced(sequencer->getTickPosition(), noteOnEvent->getNote(), noteOnEvent->getVelocity());
                sequencer->playMetronomeTrack();
            }
            else 
            {
                // Check for rec main without playing using the new refactored method
                if (SeqUtil::isRecMainWithoutPlaying(sequencer, timingCorrectScreen, currentScreenName, recButton, clientHardwareEventController))
                {
                    recordMidiNoteOn = track->recordNoteEventSynced(sequencer->getTickPosition(), noteOnEvent->getNote(), noteOnEvent->getVelocity());
                    sequencer->playMetronomeTrack();
                    
                    // Apply timing correction if needed
                    int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();
                    if (stepLength != 1)
                    {
                        int bar = sequencer->getCurrentBarIndex() + 1;
                        track->timingCorrect(0, bar, recordMidiNoteOn, stepLength, timingCorrectScreen->getSwing());
                        
                        if (recordMidiNoteOn->getTick() != sequencer->getTickPosition())
                        {
                            sequencer->move(Sequencer::ticksToQuarterNotes(recordMidiNoteOn->getTick()));
                        }
                    }
                }
            }
            
            if (recordMidiNoteOn)
            {
                noteEventStore.storeRecordNoteEvent(std::pair<int, int>(trackIndex, recordMidiNoteOn->getNote()), recordMidiNoteOn);
            }
        }
    }
    else if (type == MessageType::NOTE_OFF)
    {
        const int note = e.getNoteNumber();
        
        // Derive track information based on recording mode
        int trackIndex;
        if (sequencer->isRecordingModeMulti())
        {
            // In multi recording mode, use MultiRecordingSetupScreen to map MIDI channel to track
            auto mrsLines = multiRecordingSetupScreen->getMrsLines();
            if (e.getChannel() < mrsLines.size())
            {
                trackIndex = mrsLines[e.getChannel()]->getTrack();
            }
            else
            {
                // Channel out of range, skip processing
                return;
            }
        }
        else
        {
            // In single recording mode, use active track
            trackIndex = sequencer->getActiveTrackIndex();
        }
        
        // Get track information
        auto sequence = sequencer->isPlaying() ? sequencer->getCurrentlyPlayingSequence() : sequencer->getActiveSequence();
        auto track = sequence->getTrack(trackIndex);
        
        const int trackDevice = track->getDeviceIndex();
        const std::optional<int> drumIndex = track->getBus() > 0 ? std::optional<int>(track->getBus() - 1) : std::nullopt;
        
        // Handle recording note finalization
        if (auto storedRecordMidiNoteOn = noteEventStore.retrieveRecordNoteEvent(std::pair<int, int>(trackIndex, note)))
        {
            if (sequencer->isRecordingOrOverdubbing())
            {
                track->finalizeNoteEventASync(storedRecordMidiNoteOn);
            }
            else if (SeqUtil::isStepRecording(currentScreenName, sequencer))
            {
                auto newDuration = static_cast<int>(sequencer->getTickPosition());
                sequencer->stopMetronomeTrack();
                track->finalizeNoteEventSynced(storedRecordMidiNoteOn, newDuration);
            }
            else
            {
                // Check for rec main without playing using the new refactored method
                if (SeqUtil::isRecMainWithoutPlaying(sequencer, timingCorrectScreen, currentScreenName, recButton, clientHardwareEventController))
                {
                    auto newDuration = static_cast<int>(sequencer->getTickPosition());
                    sequencer->stopMetronomeTrack();
                    bool durationHasBeenAdjusted = track->finalizeNoteEventSynced(storedRecordMidiNoteOn, newDuration);
                    
                    // Advance sequencer position if needed
                    if (durationHasBeenAdjusted)
                    {
                        int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();
                        int nextPos = sequencer->getTickPosition() + stepLength;
                        int bar = sequencer->getCurrentBarIndex() + 1;
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
        }
        
        // Retrieve the stored play note on event and use its note off event
        if (auto storedNoteOnEvent = noteEventStore.retrievePlayNoteEvent(std::pair<int, int>(trackIndex, note)))
        {
            // Call the EventHandler's handleMidiInputNoteOff method with the stored note's note off event
            eventHandler->handleMidiInputNoteOff(storedNoteOnEvent->getNoteOff(), e.getBufferOffset(), trackIndex, trackDevice, drumIndex);
        }
    }
    else if (type == MessageType::CONTROLLER && e.getControllerNumber() == 7)
    {
        // TODO: handle CC7 / note variation slider
    }
    else if (type == MessageType::CHANNEL_PRESSURE)
    {
        // TODO: handle channel pressure (affects sound)
    }
    else if (type == MessageType::AFTERTOUCH)
    {
    }
    else if (type == MessageType::PROGRAM_CHANGE)
    {
        // TODO: handle program (sound bank) change
    }
    else if (type == MessageType::CONTROLLER && e.getControllerNumber() == 123)
    {
        // TODO: handle all notes off
    }
}

