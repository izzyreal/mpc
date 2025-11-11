#pragma once

#include "engine/audio/core/AudioProcessAdapter.hpp"
#include "engine/audio/core/AudioBuffer.hpp"

#include <memory>

namespace mpc
{
    class Mpc;
}
namespace mpc::lcdgui::screens
{
    class SampleScreen;
}

namespace mpc::audiomidi
{

    class MonitorInputAdapter : public engine::audio::core::AudioProcessAdapter
    {
        std::shared_ptr<lcdgui::screens::SampleScreen> sampleScreen;

    public:
        int32_t processAudio(engine::audio::core::AudioBuffer *buffer,
                             int nFrames) override;
        MonitorInputAdapter(Mpc &mpc, AudioProcess *process);
    };
} // namespace mpc::audiomidi
