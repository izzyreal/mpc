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
    class NoteEventProcessor final
    {
    public:
        static void noteOn(
            mpc::sampler::Sampler& sampler,
            mpc::engine::audio::mixer::AudioMixer& mixer,
            mpc::lcdgui::screens::MixerSetupScreen* mixerSetupScreen,
            std::vector<std::shared_ptr<mpc::engine::Voice>>& voices,
            std::vector<std::shared_ptr<mpc::engine::StereoMixer>>& stereoMixerChannels,
            std::vector<std::shared_ptr<mpc::engine::IndivFxMixer>>& indivFxMixerChannels,
            std::vector<mpc::engine::MixerInterconnection*>& mixerConnections,
            std::map<int,int>& simultA,
            std::map<int,int>& simultB,
            int drumIndex,
            int programNumber,
            int note, int velo, int varType, int varValue,
            int frameOffset, bool firstGeneration,
            int startTick, int durationFrames
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
