#pragma once
#include <memory>
#include <vector>
#include <map>

namespace mpc::lcdgui::screens { class MixerSetupScreen; }
namespace mpc::sampler { class Sampler; class NoteParameters; }
namespace mpc::engine { class Voice; class StereoMixer; class IndivFxMixer; class MixerInterconnection; }
namespace mpc::engine::audio::mixer { class AudioMixer; }

namespace mpc::engine
{
    class DrumNoteEventHandler final
    {
    public:
        static void noteOn(
            std::shared_ptr<mpc::sampler::Sampler> sampler,
            std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer,
            std::shared_ptr<mpc::lcdgui::screens::MixerSetupScreen> mixerSetupScreen,
            std::vector<std::shared_ptr<mpc::engine::Voice>>& voices,
            std::vector<std::shared_ptr<mpc::engine::StereoMixer>>& drumStereoMixerChannels,
            std::vector<std::shared_ptr<mpc::engine::IndivFxMixer>>& drumIndivFxMixerChannels,
            std::vector<mpc::engine::MixerInterconnection*>& mixerConnections,
            std::map<int,int>& simultA,
            std::map<int,int>& simultB,
            int drumIndex,
            int programIndex,
            int note,
            int velo,
            int varType,
            int varValue,
            int frameOffset,
            bool firstGeneration,
            int startTick,
            int durationFrames
        );

        static void noteOff(
            std::vector<std::shared_ptr<mpc::engine::Voice>>& voices,
            std::map<int,int>& simultA,
            std::map<int,int>& simultB,
            int drumIndex,
            int note,
            int frameOffset,
            int noteOnStartTick
        );
    };
}
