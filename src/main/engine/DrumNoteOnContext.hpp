#pragma once

#include <memory>
#include <vector>
#include <map>

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
    class StereoMixer;
    class IndivFxMixer;
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
        std::shared_ptr<mpc::sampler::Sampler> sampler;
        std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer;
        std::shared_ptr<mpc::lcdgui::screens::MixerSetupScreen>
            mixerSetupScreen;
        std::vector<std::shared_ptr<Voice>> *voices{};
        std::vector<MixerInterconnection *> *mixerConnections{};
        std::map<int, int> *simultA{};
        std::map<int, int> *simultB{};
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
