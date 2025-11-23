#pragma once

#include <memory>
#include <vector>
#include <functional>

namespace mpc::engine::audio::mixer
{
    class AudioMixer;
}
namespace mpc::engine
{
    class SequencerPlaybackEngine;
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
namespace mpc::performance
{
    class PerformanceManager;
}
namespace mpc::hardware
{
    class Slider;
}
namespace mpc::sequencer
{
    class Sequencer;
} // namespace mpc::sequencer

namespace mpc::engine
{
    class NoteRepeatProcessor
    {
    public:
        NoteRepeatProcessor(
            std::weak_ptr<sequencer::Sequencer>,
            std::shared_ptr<sampler::Sampler>,
            std::shared_ptr<audio::mixer::AudioMixer>,
            std::shared_ptr<lcdgui::screens::window::Assign16LevelsScreen>,
            std::shared_ptr<lcdgui::screens::MixerSetupScreen>,
            const std::weak_ptr<performance::PerformanceManager> &,
            std::shared_ptr<hardware::Slider>,
            std::vector<std::shared_ptr<Voice>> *,
            std::vector<MixerInterconnection *> &,
            const std::function<bool()> &isFullLevelEnabled,
            const std::function<bool()> &isSixteenLevelsEnabled);

        void process(const SequencerPlaybackEngine *, unsigned int tickPosition,
                     int durationTicks, unsigned short eventFrameOffset,
                     double tempo, float sampleRate) const;

    private:
        const std::weak_ptr<sequencer::Sequencer> sequencer;
        const std::shared_ptr<sampler::Sampler> sampler;
        const std::shared_ptr<audio::mixer::AudioMixer> mixer;
        const std::shared_ptr<lcdgui::screens::window::Assign16LevelsScreen>
            assign16LevelsScreen;
        const std::shared_ptr<lcdgui::screens::MixerSetupScreen>
            mixerSetupScreen;
        const std::weak_ptr<performance::PerformanceManager> performanceManager;
        const std::shared_ptr<hardware::Slider> hardwareSlider;
        std::vector<std::shared_ptr<Voice>> *voices;
        std::vector<MixerInterconnection *> &mixerConnections;
        const std::function<bool()> isFullLevelEnabled;
        const std::function<bool()> isSixteenLevelsEnabled;
    };
} // namespace mpc::engine
