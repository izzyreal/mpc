#include "MonitorInputAdapter.hpp"

#include <Mpc.hpp>
#include <sampler/Sampler.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SampleScreen.hpp>

#include <audio/core/ChannelFormat.hpp>

using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace ctoot::audio::core;
using namespace std;

MonitorInputAdapter::MonitorInputAdapter(mpc::Mpc& mpc, AudioProcess* process)
	: AudioProcessAdapter(process), mpc(mpc)
{
}

int32_t MonitorInputAdapter::processAudio(ctoot::audio::core::AudioBuffer* buffer, int nFrames)
{
	auto ret = process->processAudio(buffer, nFrames);

	auto sampleScreen = mpc.screens->get<SampleScreen>("sample");
    auto mode = sampleScreen->getMode();
    
	if (sampleScreen->getMode() == 0)
	{
        buffer->copy(0, 1, nFrames);
	}
	else if (sampleScreen->getMode() == 1)
	{
        buffer->copy(1, 0, nFrames);
	}

	return ret;
}
