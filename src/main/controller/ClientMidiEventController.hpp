#pragma once

#include "client/event/ClientMidiEvent.hpp"
#include "ClientMidiFootswitchAssignmentController.hpp"
#include "ClientMidiInputSynchronizationController.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"

#include "Observer.hpp"

namespace mpc::eventregistry
{
    class EventRegistry;
}

namespace mpc::sequencer
{
    class Sequencer;
    class FrameSeq;
} // namespace mpc::sequencer

namespace mpc::engine
{
    class PreviewSoundPlayer;
}

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

    class ClientMidiEventController : public Observable
    {
    private:
        using MidiEvent = mpc::client::event::ClientMidiEvent;
        using MessageType = MidiEvent::MessageType;
        using Sequencer = mpc::sequencer::Sequencer;
        using Sampler = mpc::sampler::Sampler;
        using EventHandler = mpc::audiomidi::EventHandler;
        using MidiInputScreen = mpc::lcdgui::screens::window::MidiInputScreen;
        using MultiRecordingSetupScreen =
            mpc::lcdgui::screens::window::MultiRecordingSetupScreen;
        using TimingCorrectScreen =
            mpc::lcdgui::screens::window::TimingCorrectScreen;

        using Track = mpc::sequencer::Track;
        using Program = mpc::sampler::Program;

        std::shared_ptr<ClientEventController> clientEventController;

        std::shared_ptr<ClientMidiFootswitchAssignmentController>
            footswitchController;
        ClientMidiInputSynchronizationController syncController;

        std::shared_ptr<ClientHardwareEventController>
            clientHardwareEventController;

        std::optional<int> getTrackIndexForEvent(const MidiEvent &) const;
        std::shared_ptr<Track> getTrackForEvent(const MidiEvent &) const;
        bool shouldProcessEvent(const MidiEvent &) const;

        std::optional<int> getDrumIndex(const std::shared_ptr<Track> &) const;
        std::shared_ptr<Program> getProgramForEvent(const MidiEvent &) const;
        std::optional<int> getDrumIndexForEvent(const MidiEvent &) const;
        std::shared_ptr<sequencer::DrumBus>
        getDrumBusForEvent(const MidiEvent &) const;

        bool isChannelMatch(const MidiEvent &e) const noexcept;
        bool isOmniOn() const noexcept;
        bool isPolyMode() const noexcept;

        std::shared_ptr<eventregistry::EventRegistry> eventRegistry;
        std::shared_ptr<MidiInputScreen> midiInputScreen;
        std::shared_ptr<EventHandler> eventHandler;
        std::shared_ptr<Sequencer> sequencer;
        std::shared_ptr<Sampler> sampler;
        std::shared_ptr<MultiRecordingSetupScreen> multiRecordingSetupScreen;
        std::shared_ptr<TimingCorrectScreen> timingCorrectScreen;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<hardware::Hardware> hardware;
        std::shared_ptr<lcdgui::Screens> screens;
        std::shared_ptr<sequencer::FrameSeq> frameSequencer;
        engine::PreviewSoundPlayer *previewSoundPlayer;

    public:
        ClientMidiEventController(
            std::shared_ptr<eventregistry::EventRegistry>,
            std::shared_ptr<ClientEventController>,
            std::shared_ptr<ClientHardwareEventController>,
            std::shared_ptr<MidiSwScreen>, std::shared_ptr<Sequencer>,
            std::shared_ptr<Sampler>, std::shared_ptr<MidiInputScreen>,
            std::shared_ptr<EventHandler>,
            std::shared_ptr<MultiRecordingSetupScreen>,
            std::shared_ptr<TimingCorrectScreen>,
            std::shared_ptr<lcdgui::LayeredScreen>,
            std::shared_ptr<hardware::Hardware>,
            std::shared_ptr<lcdgui::Screens>,
            std::shared_ptr<sequencer::FrameSeq>, engine::PreviewSoundPlayer *);

        std::shared_ptr<ClientMidiFootswitchAssignmentController>
        getFootswitchAssignmentController();

        void handleClientMidiEvent(const MidiEvent &);

    private:
        bool convertSustainPedalToDuration = false;

        std::unordered_map<int, bool> sustainPedalState;
        std::unordered_map<int, std::vector<int>> heldNotes;
        std::unordered_map<int, std::vector<int>> sustainedNotes;

        void handleNoteOn(const MidiEvent &e);
        void handleNoteOff(const MidiEvent &e);
        void handleKeyAftertouch(const MidiEvent &e);
        void handleChannelAftertouch(const MidiEvent &e);
        void handlePitchBend(const MidiEvent &e);
        void handleControlChange(const MidiEvent &e);
        void handleProgramChange(const MidiEvent &e);

        // System Exclusive (Manufacturer ID: 0x45)
        void handleSystemExclusive(const MidiEvent &e);

        // System Common
        void handleSongPositionPointer(const MidiEvent &e);
        void handleSongSelect(const MidiEvent &e);
        void handleTuneRequest(const MidiEvent &e);

        // System Real-Time
        void handleMidiClock(const MidiEvent &e);
        void handleStart(const MidiEvent &e);
        void handleStop(const MidiEvent &e);
        void handleContinue(const MidiEvent &e);

        // Auxiliary / Non-Recognized Messages
        void handleLocalOnOff(const MidiEvent &e);
        void handleAllNotesOff(const MidiEvent &e);
        void handleActiveSense(const MidiEvent &e);
        void handleReset(const MidiEvent &e);

        void noteOnInternal(int channel, int note, int velocity);
        void noteOffInternal(int channel, int note);

        void holdNoteForSustain(int channel, int note);
        void releaseSustainedNotes(int channel);

        void enforceMonoMode(int channel, int newNote);
    };
} // namespace mpc::controller
