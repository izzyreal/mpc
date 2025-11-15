#pragma once

#include "command/context/TriggerLocalNoteOnContext.hpp"
#include "command/context/TriggerLocalNoteOffContext.hpp"

#include "IntTypes.hpp"

#include <memory>

namespace mpc::engine
{
    class SequencerPlaybackEngine;
}

namespace mpc::sequencer
{
    class Track;
    class Sequencer;
} // namespace mpc::sequencer

namespace mpc::eventregistry
{
    class PerformanceManager;
}

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::hardware
{
    class Hardware;
}

namespace mpc::controller
{
    class ClientEventController;
}

namespace mpc::lcdgui
{
    class ScreenComponent;
    class Screens;
} // namespace mpc::lcdgui

namespace mpc::command::context
{
    class TriggerLocalNoteContextFactory
    {
    public:
        static std::shared_ptr<TriggerLocalNoteOnContext>
        buildTriggerLocalNoteOnContext(
            eventregistry::Source, const eventregistry::NoteOnEvent &,
            NoteNumber note, Velocity velocity, sequencer::Track *,
            const std::shared_ptr<sequencer::Bus> &,
            const std::shared_ptr<lcdgui::ScreenComponent> &,
            ProgramPadIndex programPadIndex,
            const std::shared_ptr<sampler::Program> &,
            const std::shared_ptr<sequencer::Sequencer> &,
            const std::shared_ptr<engine::SequencerPlaybackEngine> &,
            const std::shared_ptr<eventregistry::PerformanceManager> &,
            const std::shared_ptr<controller::ClientEventController> &,
            const std::shared_ptr<audiomidi::EventHandler> &,
            const std::shared_ptr<lcdgui::Screens> &,
            const std::shared_ptr<hardware::Hardware> &);

        static std::shared_ptr<TriggerLocalNoteOffContext>
        buildTriggerLocalNoteOffContext(
            eventregistry::Source source, NoteNumber note,
            NoteEventId recordedNoteOnEventId, sequencer::Track *,
            sequencer::BusType,
            const std::shared_ptr<lcdgui::ScreenComponent> &,
            ProgramPadIndex programPadIndex,
            const std::shared_ptr<sampler::Program> &,
            const std::shared_ptr<sequencer::Sequencer> &,
            const std::shared_ptr<engine::SequencerPlaybackEngine> &,
            const std::shared_ptr<eventregistry::PerformanceManager> &,
            const std::shared_ptr<controller::ClientEventController> &,
            const std::shared_ptr<audiomidi::EventHandler> &eventHandler,
            const std::shared_ptr<lcdgui::Screens> &,
            const std::shared_ptr<hardware::Hardware> &);
    };
} // namespace mpc::command::context
