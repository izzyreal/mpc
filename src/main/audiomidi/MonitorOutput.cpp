#include "MonitorOutput.hpp"

#include <Mpc.hpp>
#include <sampler/Sampler.hpp>
#include <ui/sampler/SamplerGui.hpp>

#include <audio/core/ChannelFormat.hpp>

using namespace mpc::audiomidi;

using namespace std;

MonitorOutput::MonitorOutput()
{
	
}

string MonitorOutput::getName()
{
    return "monitor";
}

void MonitorOutput::open()
{
    closed = false;
}

int32_t MonitorOutput::processAudio(ctoot::audio::core::AudioBuffer* buffer)
{
	/*
	auto sampler = Mpc::instance().getSampler().lock();
	
	if (!sampler) {
		return AUDIO_SILENCE;
	}

	if (sampler->monitorBufferL == nullptr ||
		sampler->monitorBufferR == nullptr ||
		Mpc::instance().getUis().lock()->getSamplerGui()->getMonitor() == 0 ||
		Mpc::instance().getLayeredScreen().lock()->getCurrentScreenName().compare("sample") != 0 ||
		closed) {
		buffer->makeSilence();
		return AUDIO_SILENCE;
	}
	auto mode = Mpc::instance().getUis().lock()->getSamplerGui()->getMode();
	auto left = buffer->getChannel(0);
	auto right = buffer->getChannel(1);
	for (int i = 0; i < buffer->getSampleCount(); i++) {
		(*left)[i] = mode != 1 ? (*sampler->monitorBufferL)[i] : (*sampler->monitorBufferR)[i];
		(*right)[i] = mode != 0 ? (*sampler->monitorBufferR)[i] : (*sampler->monitorBufferL)[i];
	}
	*/
	return AUDIO_OK;
}

void MonitorOutput::close()
{
    closed = true;
}

string MonitorOutput::getLocation()
{
    return "sampler";
}

MonitorOutput::~MonitorOutput() {
}
