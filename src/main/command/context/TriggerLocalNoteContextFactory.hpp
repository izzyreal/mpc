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

namespace mpc::sampler
{
    class Sampler;
}

namespace mpc::eventregistry
{
    class EventRegistry;
}

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::engine
{
    class PreviewSoundPlayer;
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
    class LayeredScreen;
    class ScreenComponent;
    class Screens;
} // namespace mpc::lcdgui

namespace mpc::command::context
{
    class TriggerLocalNoteContextFactory
    {
    public:
        static std::shared_ptr<TriggerLocalNoteOnContext>
        buildTriggerDrumNoteOnContext(
            eventregistry::Source, std::shared_ptr<lcdgui::LayeredScreen>,
            std::shared_ptr<controller::ClientEventController>,
            std::shared_ptr<hardware::Hardware>,
            std::shared_ptr<sequencer::Sequencer>,
            std::shared_ptr<lcdgui::Screens>, std::shared_ptr<sampler::Sampler>,
            std::shared_ptr<eventregistry::EventRegistry>,
            std::shared_ptr<audiomidi::EventHandler>,
            std::shared_ptr<sequencer::FrameSeq>, engine::PreviewSoundPlayer *,
            const int programPadIndex, const int velocity,
            const std::shared_ptr<mpc::lcdgui::ScreenComponent>);

        static std::shared_ptr<TriggerLocalNoteOffContext>
        buildTriggerDrumNoteOffContext(
            eventregistry::Source, engine::PreviewSoundPlayer *,
            std::shared_ptr<eventregistry::EventRegistry>,
            std::shared_ptr<audiomidi::EventHandler>,
            std::shared_ptr<lcdgui::Screens>,
            std::shared_ptr<sequencer::Sequencer>,
            std::shared_ptr<hardware::Hardware>,
            std::shared_ptr<controller::ClientEventController>,
            std::shared_ptr<sequencer::FrameSeq>, const int programPadIndex,
            int drumIndex, const std::shared_ptr<mpc::lcdgui::ScreenComponent>,
            const int note, std::shared_ptr<sampler::Program>,
            sequencer::Track *);
    };
} // namespace mpc::command::context
