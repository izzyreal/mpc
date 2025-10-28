#pragma once

#include "client/event/ClientMidiEvent.hpp"

#include <memory>
#include <optional>

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

namespace mpc::sequencer
{
    class Sequencer;
    class Track;
    class DrumBus;
} // namespace mpc::sequencer

namespace mpc::sampler
{
    class Sampler;
    class Program;
}

namespace mpc::controller
{
    class ClientEventController;

    class ClientMidiSoundGeneratorController
    {
    private:
        using MidiEvent = mpc::client::event::ClientMidiEvent;
        using Sequencer = mpc::sequencer::Sequencer;
        using Sampler = mpc::sampler::Sampler;
        using Track = mpc::sequencer::Track;
        using EventHandler = mpc::audiomidi::EventHandler;
        using Program = mpc::sampler::Program;
        using MidiInputScreen = mpc::lcdgui::screens::window::MidiInputScreen;
        using MultiRecordingSetupScreen =
            mpc::lcdgui::screens::window::MultiRecordingSetupScreen;
        using TimingCorrectScreen =
            mpc::lcdgui::screens::window::TimingCorrectScreen;
        using NoteKey = std::pair<int /*track*/, int /*note*/>;

    public:
        ClientMidiSoundGeneratorController(
            std::shared_ptr<ClientEventController>,
            std::shared_ptr<MidiInputScreen>, std::shared_ptr<EventHandler>,
            std::shared_ptr<Sequencer>,
            std::shared_ptr<Sampler>,
            std::shared_ptr<MultiRecordingSetupScreen>,
            std::shared_ptr<TimingCorrectScreen>);

        void handleEvent(const MidiEvent &);

    private:
        std::shared_ptr<MidiInputScreen> midiInputScreen;
        std::shared_ptr<EventHandler> eventHandler;
        std::shared_ptr<Sequencer> sequencer;
        std::shared_ptr<Sampler> sampler;
        std::shared_ptr<MultiRecordingSetupScreen> multiRecordingSetupScreen;
        std::shared_ptr<TimingCorrectScreen> timingCorrectScreen;
        std::shared_ptr<ClientEventController> clientEventController;

        std::optional<int> getTrackIndexForEvent(const MidiEvent &) const;
        std::shared_ptr<Track> getTrackForEvent(const MidiEvent &) const;
        bool shouldProcessEvent(const MidiEvent &) const;

        std::optional<int> getDrumIndex(const std::shared_ptr<Track> &) const;
        std::shared_ptr<Program> getProgramForEvent(const MidiEvent &) const;
        std::optional<int> getDrumIndexForEvent(const MidiEvent &) const;
        std::shared_ptr<sequencer::DrumBus> getDrumBusForEvent(const MidiEvent &) const;

        void handleNoteOnEvent(const MidiEvent &);
        void handleNoteOffEvent(const MidiEvent &);
    };
} // namespace mpc::controller
