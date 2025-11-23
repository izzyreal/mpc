#pragma once

#include "ClientMidiFootswitchAssignmentController.hpp"
#include "ClientMidiInputSynchronizationController.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"

#include "Observer.hpp"
#include "sequencer/BusType.hpp"

namespace mpc::file::mid::event
{
    class ClientMidiEvent;
}
namespace mpc::performance
{
    class PerformanceManager;
}

namespace mpc::sequencer
{
    class Sequencer;
} // namespace mpc::sequencer

namespace mpc::engine
{
    class SequencerPlaybackEngine;
    class PreviewSoundPlayer;
} // namespace mpc::engine

namespace mpc::sampler
{
    class Sampler;
}

namespace mpc::lcdgui
{
    class Screens;
    class LayeredScreen;
} // namespace mpc::lcdgui

namespace mpc::lcdgui::screens::window
{
    class MidiInputScreen;
    class MultiRecordingSetupScreen;
    class TimingCorrectScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::hardware
{
    class Hardware;
    class Button;
} // namespace mpc::hardware

namespace mpc::controller
{
    class ClientEventController;
    class ClientHardwareEventController;

    class ClientMidiEventController final : public Observable
    {
        using ClientMidiEvent = client::event::ClientMidiEvent;
        using MessageType = ClientMidiEvent::MessageType;
        using Sequencer = sequencer::Sequencer;
        using Sampler = sampler::Sampler;
        using EventHandler = audiomidi::EventHandler;
        using MidiInputScreen = MidiInputScreen;
        using MultiRecordingSetupScreen = MultiRecordingSetupScreen;
        using TimingCorrectScreen = TimingCorrectScreen;

        using Track = sequencer::Track;
        using Program = sampler::Program;

    public:
        ClientMidiEventController(
            const std::weak_ptr<performance::PerformanceManager> &,
            const std::shared_ptr<ClientEventController> &,
            std::shared_ptr<ClientHardwareEventController>,
            std::shared_ptr<MidiSwScreen>, std::weak_ptr<Sequencer>,
            const std::shared_ptr<Sampler> &,
            const std::shared_ptr<MidiInputScreen> &,
            const std::shared_ptr<audiomidi::EventHandler> &,
            const std::shared_ptr<MultiRecordingSetupScreen> &,
            const std::shared_ptr<TimingCorrectScreen> &,
            const std::shared_ptr<lcdgui::LayeredScreen> &,
            const std::shared_ptr<hardware::Hardware> &,
            const std::shared_ptr<lcdgui::Screens> &,
            const std::shared_ptr<engine::SequencerPlaybackEngine> &,
            engine::PreviewSoundPlayer *);

        std::shared_ptr<ClientMidiFootswitchAssignmentController>
        getFootswitchAssignmentController();

        void handleClientMidiEvent(const ClientMidiEvent &);

    private:
        std::shared_ptr<ClientEventController> clientEventController;

        std::shared_ptr<ClientMidiFootswitchAssignmentController>
            footswitchController;
        ClientMidiInputSynchronizationController syncController;

        std::shared_ptr<ClientHardwareEventController>
            clientHardwareEventController;

        std::optional<int> getTrackIndexForEvent(const ClientMidiEvent &) const;
        std::shared_ptr<Track> getTrackForEvent(const ClientMidiEvent &) const;
        bool shouldProcessEvent(const ClientMidiEvent &) const;

        std::shared_ptr<Program>
        getProgramForEvent(const ClientMidiEvent &) const;
        ProgramIndex getProgramIndexForEvent(const ClientMidiEvent &) const;
        std::optional<sequencer::BusType>
        getDrumBusTypeForEvent(const ClientMidiEvent &) const;
        std::shared_ptr<sequencer::DrumBus>
        getDrumBusForEvent(const ClientMidiEvent &) const;

        bool isOmniOn() const noexcept;
        bool isPolyMode() const noexcept;

        std::weak_ptr<performance::PerformanceManager> performanceManager;
        std::shared_ptr<MidiInputScreen> midiInputScreen;
        std::shared_ptr<EventHandler> eventHandler;
        std::weak_ptr<Sequencer> sequencer;
        std::shared_ptr<Sampler> sampler;
        std::shared_ptr<MultiRecordingSetupScreen> multiRecordingSetupScreen;
        std::shared_ptr<TimingCorrectScreen> timingCorrectScreen;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<hardware::Hardware> hardware;
        std::shared_ptr<lcdgui::Screens> screens;
        std::shared_ptr<engine::SequencerPlaybackEngine>
            sequencerPlaybackEngine;
        engine::PreviewSoundPlayer *previewSoundPlayer;

        bool convertSustainPedalToDuration = false;

        std::unordered_map<int, bool> sustainPedalState;
        std::unordered_map<int, std::vector<int>> heldNotes;
        std::unordered_map<int, std::vector<int>> sustainedNotes;

        void handleNoteOn(const ClientMidiEvent &e);
        void handleNoteOff(const ClientMidiEvent &e);
        void handleKeyAftertouch(const ClientMidiEvent &e) const;
        void handleChannelAftertouch(const ClientMidiEvent &e) const;
        void handlePitchBend(const ClientMidiEvent &e) const;
        void handleControlChange(const ClientMidiEvent &e);
        void handleProgramChange(const ClientMidiEvent &e) const;

        // System Exclusive (Manufacturer ID: 0x45)
        void handleSystemExclusive(const ClientMidiEvent &e) const;

        // System Common
        void handleSongPositionPointer(const ClientMidiEvent &e) const;
        void handleSongSelect(const ClientMidiEvent &e) const;
        void handleTuneRequest(const ClientMidiEvent &e) const;

        // System Real-Time
        void handleMidiClock(const ClientMidiEvent &e) const;
        void handleStart(const ClientMidiEvent &e) const;
        void handleStop(const ClientMidiEvent &e) const;
        void handleContinue(const ClientMidiEvent &e) const;

        // Auxiliary / Non-Recognized Messages
        void handleLocalOnOff(const ClientMidiEvent &e) const;
        void handleAllNotesOff(const ClientMidiEvent &e) const;
        void handleActiveSense(const ClientMidiEvent &e) const;
        void handleReset(const ClientMidiEvent &e) const;

        void noteOnInternal(int channel, int note, int velocity);
        void noteOffInternal(int channel, int note);

        void holdNoteForSustain(int channel, int note);
        void releaseSustainedNotes(int channel);

        void enforceMonoMode(int channel, int newNote);
    };
} // namespace mpc::controller
