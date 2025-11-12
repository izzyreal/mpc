#include "MonitorInputAdapter.hpp"

#include "Mpc.hpp"
#include <SampleOps.hpp>

#include "lcdgui/screens/SampleScreen.hpp"

using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::engine::audio::core;

MonitorInputAdapter::MonitorInputAdapter(Mpc &mpc, AudioProcess *process)
    : AudioProcessAdapter(process),
      sampleScreen(mpc.screens->get<ScreenId::SampleScreen>())
{
}

int32_t MonitorInputAdapter::processAudio(AudioBuffer *buffer, int nFrames)
{
    const auto ret = process->processAudio(buffer, nFrames);

    if (sampleScreen->getMode() == 0)
    {
        sampleops::clamp_mean_normalized_floats(buffer->getChannel(0));
        buffer->copyChannel(0, 1);
    }
    else if (sampleScreen->getMode() == 1)
    {
        sampleops::clamp_mean_normalized_floats(buffer->getChannel(1));
        buffer->copyChannel(1, 0);
    }
    else
    {
        sampleops::clamp_mean_normalized_floats(buffer->getChannel(0));
        sampleops::clamp_mean_normalized_floats(buffer->getChannel(1));
    }

    return ret;
}
