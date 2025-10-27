#pragma once

#include "client/event/ClientMidiEvent.hpp"

#include "sequencer/NoteEventStore.hpp"

#include <memory>

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
} // namespace mpc::sequencer

namespace mpc::hardware
{
    class Button;
}

namespace mpc::controller
{
    class ClientEventController;
}

namespace mpc::controller
{
    class ClientMidiSoundGeneratorController
    {
    public:
        ClientMidiSoundGeneratorController(
            std::shared_ptr<mpc::controller::ClientEventController>,
            std::shared_ptr<mpc::lcdgui::screens::window::MidiInputScreen>,
            std::shared_ptr<mpc::audiomidi::EventHandler>,
            std::shared_ptr<mpc::sequencer::Sequencer>,
            std::shared_ptr<mpc::lcdgui::screens::window::MultiRecordingSetupScreen>,
            std::shared_ptr<mpc::lcdgui::screens::window::TimingCorrectScreen>);

        void handleEvent(const mpc::client::event::ClientMidiEvent &);
        void clearNoteEventStore();

    private:
        std::shared_ptr<mpc::lcdgui::screens::window::MidiInputScreen>
            midiInputScreen;
        std::shared_ptr<mpc::audiomidi::EventHandler> eventHandler;
        std::shared_ptr<mpc::sequencer::Sequencer> sequencer;
        std::shared_ptr<mpc::lcdgui::screens::window::MultiRecordingSetupScreen>
            multiRecordingSetupScreen;
        std::shared_ptr<mpc::lcdgui::screens::window::TimingCorrectScreen>
            timingCorrectScreen;
        std::shared_ptr<mpc::controller::ClientEventController>
            clientEventController;

        const size_t NOTE_EVENT_STORE_CAPACITY = 8192;
        mpc::sequencer::NoteEventStore<std::pair<int /*track*/, int /*note*/>>
            noteEventStore;

        std::optional<int> getTrackIndexForEvent(
            const mpc::client::event::ClientMidiEvent &) const;
        std::shared_ptr<mpc::sequencer::Track>
        getTrackForIndex(int trackIndex) const;
        bool
        shouldProcessEvent(const mpc::client::event::ClientMidiEvent &) const;

        void handleNoteOnEvent(const mpc::client::event::ClientMidiEvent &);
        void handleNoteOffEvent(const mpc::client::event::ClientMidiEvent &);
    };
} // namespace mpc::controller
