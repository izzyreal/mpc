#pragma once

#include <engine/audio/core/AudioProcessAdapter.hpp>
#include <engine/audio/core/AudioBuffer.hpp>

#include <string>
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

    class MonitorInputAdapter
        : public mpc::engine::audio::core::AudioProcessAdapter
    {
    private:
        std::shared_ptr<mpc::lcdgui::screens::SampleScreen> sampleScreen;

    public:
        int32_t processAudio(mpc::engine::audio::core::AudioBuffer *buffer, int nFrames) override;
        MonitorInputAdapter(mpc::Mpc &mpc, mpc::engine::audio::core::AudioProcess *process);
    };
} // namespace mpc::audiomidi
