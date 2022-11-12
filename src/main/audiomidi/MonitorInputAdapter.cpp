#include "MonitorInputAdapter.hpp"

#include <Mpc.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/SampleScreen.hpp>

#include <audio/core/ChannelFormat.hpp>

using namespace mpc::audiomidi;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace ctoot::audio::core;

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
        buffer->copyChannel(0, 1);
	}
	else if (sampleScreen->getMode() == 1)
	{
        buffer->copyChannel(1, 0);
	}

	return ret;
}
