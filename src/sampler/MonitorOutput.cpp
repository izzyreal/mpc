#include <sampler/MonitorOutput.hpp>

#include <Mpc.hpp>
#include <sampler/Sampler.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Sampler.hpp>
#include <audio/core/ChannelFormat.hpp>

using namespace mpc::sampler;
using namespace std;

MonitorOutput::MonitorOutput(mpc::Mpc* mpc)
{
	this->mpc = mpc;
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
	auto sampler = mpc->getSampler().lock();
	if (!sampler) return AUDIO_SILENCE;
	if (sampler->monitorBufferL == nullptr || sampler->monitorBufferR == nullptr || mpc->getUis().lock()->getSamplerGui()->getMonitor() == 0 || mpc->getLayeredScreen().lock()->getCurrentScreenName().compare("sample") != 0 || closed) {
		buffer->makeSilence();
		return AUDIO_SILENCE;
	}
	auto mode = mpc->getUis().lock()->getSamplerGui()->getMode();
	auto left = buffer->getChannel(0);
	auto right = buffer->getChannel(1);
	for (int i = 0; i < buffer->getSampleCount(); i++) {
		(*left)[i] = mode != 1 ? (*sampler->monitorBufferL)[i] : (*sampler->monitorBufferR)[i];
		(*right)[i] = mode != 0 ? (*sampler->monitorBufferR)[i] : (*sampler->monitorBufferL)[i];
	}
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
