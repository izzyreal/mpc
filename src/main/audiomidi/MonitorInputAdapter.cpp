#include "MonitorInputAdapter.hpp"

#include <Mpc.hpp>
#include <SampleOps.hpp>

#include <lcdgui/screens/SampleScreen.hpp>

using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::engine::audio::core;

MonitorInputAdapter::MonitorInputAdapter(mpc::Mpc& mpc, AudioProcess* process)
	: AudioProcessAdapter(process), sampleScreen(mpc.screens->get<SampleScreen>())
{
}

int32_t MonitorInputAdapter::processAudio(mpc::engine::audio::core::AudioBuffer* buffer, int nFrames)
{
    const auto ret = process->processAudio(buffer, nFrames);

    if (sampleScreen->getMode() == 0)
    {
        mpc::sampleops::clamp_mean_normalized_floats(buffer->getChannel(0));
        buffer->copyChannel(0, 1);
    }
    else if (sampleScreen->getMode() == 1)
    {
        mpc::sampleops::clamp_mean_normalized_floats(buffer->getChannel(1));
        buffer->copyChannel(1, 0);
    }
    else
    {
        mpc::sampleops::clamp_mean_normalized_floats(buffer->getChannel(0));
        mpc::sampleops::clamp_mean_normalized_floats(buffer->getChannel(1));
    }

    return ret;
}

