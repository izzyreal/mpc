#pragma once

#include <memory>
#include <vector>

namespace mpc::sampler
{
    class Sampler;
}
namespace mpc::lcdgui::screens
{
    class MixerSetupScreen;
}
namespace mpc::engine
{
    class Voice;
    class MixerInterconnection;
} // namespace mpc::engine
namespace mpc::engine::audio::mixer
{
    class AudioMixer;
}
namespace mpc::sequencer
{
    class DrumBus;
}

namespace mpc::engine
{
    struct DrumNoteOnContext
    {
        uint64_t noteEventId;
        std::shared_ptr<sampler::Sampler> sampler;
        std::shared_ptr<audio::mixer::AudioMixer> mixer;
        std::shared_ptr<lcdgui::screens::MixerSetupScreen> mixerSetupScreen;
        std::vector<std::shared_ptr<Voice>> *voices{};
        std::vector<MixerInterconnection *> *mixerConnections{};
        std::shared_ptr<sequencer::DrumBus> drum;
        int note{};
        int velocity{};
        int varType{};
        int varValue{};
        int frameOffset{};
        bool firstGeneration{};
        int startTick{};
        int durationFrames{};
    };
} // namespace mpc::engine
