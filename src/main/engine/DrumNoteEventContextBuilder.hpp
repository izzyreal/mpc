#pragma once

#include "DrumNoteOnContext.hpp"
#include "DrumNoteOffContext.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{
    class DrumBus;
}

namespace mpc::engine
{
    class Voice;
} // namespace mpc::engine
namespace mpc::sampler
{
    class Sampler;
}
namespace mpc::engine::audio::mixer
{
    class AudioMixer;
}
namespace mpc::lcdgui::screens
{
    class MixerSetupScreen;
}

namespace mpc::engine
{
    class DrumNoteEventContextBuilder
    {
    public:
        static DrumNoteOnContext buildNoteOn(
            uint64_t noteEventId, std::shared_ptr<sequencer::DrumBus>,
            std::shared_ptr<mpc::sampler::Sampler> sampler,
            std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer,
            std::shared_ptr<mpc::lcdgui::screens::MixerSetupScreen>
                mixerSetupScreen,
            std::vector<std::shared_ptr<Voice>> *voices,
            std::vector<MixerInterconnection *> &mixerConnections, int note,
            int velocity, int varType, int varValue, int frameOffset,
            bool firstGeneration, int startTick, int durationFrames);

        static DrumNoteOffContext
        buildNoteOff(uint64_t noteEventId,
                     std::shared_ptr<sequencer::DrumBus> drum,
                     std::vector<std::shared_ptr<Voice>> *voices, int note,
                     int noteOnStartTick);
    };
} // namespace mpc::engine
