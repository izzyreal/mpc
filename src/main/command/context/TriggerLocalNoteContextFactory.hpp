#pragma once

#include "command/context/TriggerLocalNoteOnContext.hpp"
#include "command/context/TriggerLocalNoteOffContext.hpp"

#include "IntTypes.hpp"
#include "sequencer/BusType.hpp"

#include <memory>

namespace mpc::sequencer
{
    class Track;
    class Sequencer;
} // namespace mpc::sequencer

namespace mpc::performance
{
    struct NoteOnEvent;
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
            performance::PerformanceEventSource,
            const performance::NoteOnEvent &, NoteNumber note,
            Velocity velocity, sequencer::Track *,
            const std::shared_ptr<sequencer::Bus> &,
            const std::shared_ptr<lcdgui::ScreenComponent> &,
            ProgramPadIndex programPadIndex,
            const std::shared_ptr<sampler::Program> &,
            const std::weak_ptr<sequencer::Sequencer> &,
            const std::weak_ptr<performance::PerformanceManager> &,
            const std::shared_ptr<controller::ClientEventController> &,
            const std::shared_ptr<audiomidi::EventHandler> &,
            const std::shared_ptr<lcdgui::Screens> &,
            const std::shared_ptr<hardware::Hardware> &);

        static std::shared_ptr<TriggerLocalNoteOffContext>
        buildTriggerLocalNoteOffContext(
            performance::PerformanceEventSource source, NoteNumber noteNumber,
            const sequencer::EventData *recordedNoteOnEvent, sequencer::Track *,
            sequencer::BusType,
            const std::shared_ptr<lcdgui::ScreenComponent> &,
            ProgramPadIndex programPadIndex,
            const std::shared_ptr<sampler::Program> &,
            const std::weak_ptr<sequencer::Sequencer> &,
            const std::weak_ptr<performance::PerformanceManager> &,
            const std::shared_ptr<controller::ClientEventController> &,
            const std::shared_ptr<audiomidi::EventHandler> &eventHandler,
            const std::shared_ptr<lcdgui::Screens> &,
            const std::shared_ptr<hardware::Hardware> &);
    };
} // namespace mpc::command::context
