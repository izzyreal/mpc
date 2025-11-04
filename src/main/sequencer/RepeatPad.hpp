#pragma once

#include <memory>
#include <vector>

namespace mpc::engine::audio::mixer
{
    class AudioMixer;
}

namespace mpc::engine
{
    class Voice;
    class MixerInterconnection;
} // namespace mpc::engine

namespace mpc::sampler
{
    class Sampler;
}

namespace mpc::lcdgui::screens::window
{
    class Assign16LevelsScreen;
}

namespace mpc::lcdgui::screens
{
    class MixerSetupScreen;
}

namespace mpc::eventregistry
{
    class EventRegistry;
}

namespace mpc::hardware
{
    class Slider;
}

namespace mpc::sequencer
{
    class Sequencer;
    class FrameSeq;
    class RepeatPad
    {
    public:
        static void
        process(FrameSeq *, std::shared_ptr<Sequencer>,
                std::shared_ptr<sampler::Sampler>,
                std::shared_ptr<engine::audio::mixer::AudioMixer>,
                bool isFullLevelEnabled, bool isSixteenLevelsEnabled,
                std::shared_ptr<lcdgui::screens::window::Assign16LevelsScreen>,
                std::shared_ptr<lcdgui::screens::MixerSetupScreen>,
                std::shared_ptr<eventregistry::EventRegistry>,
                std::shared_ptr<hardware::Slider>,
                std::vector<std::shared_ptr<engine::Voice>> *,
                std::vector<engine::MixerInterconnection *> &,
                unsigned int tickPosition, int durationTicks,
                unsigned short eventFrameOffset, double tempo,
                float sampleRate);
    };
} // namespace mpc::sequencer
