#pragma once

#include "command/context/TriggerLocalNoteOnContext.hpp"
#include "command/context/TriggerLocalNoteOffContext.hpp"

#include <memory>

namespace mpc::sequencer
{
    class Track;
    class Sequencer;
    class FrameSeq;
} // namespace mpc::sequencer

namespace mpc::eventregistry
{
    class EventRegistry;
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
            eventregistry::Source,
            eventregistry::NoteOnEvent *registryNoteOnEvent, const int note,
            const int velocity, sequencer::Track *,
            std::shared_ptr<sequencer::Bus>,
            const std::shared_ptr<lcdgui::ScreenComponent>,
            std::optional<int> programPadIndex,
            std::shared_ptr<sampler::Program>,
            std::shared_ptr<sequencer::Sequencer>,
            std::shared_ptr<sequencer::FrameSeq>,
            std::shared_ptr<eventregistry::EventRegistry>,
            std::shared_ptr<controller::ClientEventController>,
            std::shared_ptr<audiomidi::EventHandler>,
            std::shared_ptr<lcdgui::Screens>,
            std::shared_ptr<hardware::Hardware>);

        static std::shared_ptr<TriggerLocalNoteOffContext>
        buildTriggerLocalNoteOffContext(
            eventregistry::Source source, const int note, sequencer::Track *,
            std::shared_ptr<sequencer::Bus>,
            const std::shared_ptr<lcdgui::ScreenComponent>,
            std::optional<int> programPadIndex,
            std::shared_ptr<sampler::Program>,
            std::shared_ptr<sequencer::Sequencer>,
            std::shared_ptr<sequencer::FrameSeq>,
            std::shared_ptr<eventregistry::EventRegistry>,
            std::shared_ptr<controller::ClientEventController>,
            std::shared_ptr<audiomidi::EventHandler> eventHandler,
            std::shared_ptr<lcdgui::Screens>,
            std::shared_ptr<hardware::Hardware>);
    };
} // namespace mpc::command::context
