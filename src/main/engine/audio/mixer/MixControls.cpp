#include <engine/audio/mixer/MixControls.hpp>

#include <engine/audio/core/ChannelFormat.hpp>
#include "engine/FaderControl.hpp"
#include <engine/audio/mixer/BusControls.hpp>
#include <engine/audio/mixer/MixControlIds.hpp>
#include <engine/audio/mixer/BalanceControl.hpp>
#include <engine/audio/mixer/PanControl.hpp>
#include <engine/audio/mixer/MixerControls.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>
#include <engine/control/Control.hpp>
#include <engine/control/EnumControl.hpp>
#include <engine/control/BooleanControl.hpp>

#include <cmath>

using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::audio::core;
using namespace mpc::engine;
using namespace mpc::engine::control;

using namespace std;

MixControls::MixControls(MixerControls* mixerControls, int stripId, shared_ptr<BusControls> busControls, bool master)
	: AudioControls(busControls->getId(), busControls->getName())
{
	this->mixerControls = mixerControls;
	this->busControls = busControls;
	this->master = master;
	gainControl = shared_ptr<FaderControl>(mixerControls->createFaderControl());
	auto busId = busControls->getId();
	auto format = getChannelFormat();
	channelCount = format->getCount();

	if (channelCount > 1) {
		if (stripId == MixerControlsIds::CHANNEL_STRIP) {
			lcrControl = make_shared<PanControl>();
			add(lcrControl);
		}
		else {
			lcrControl = make_shared<BalanceControl>();
			add(lcrControl);
		}
		derive(lcrControl.get());
	}

	muteControl = shared_ptr<BooleanControl>(createMuteControl());
	derive(muteControl.get());
	add(muteControl);

	gainControl = shared_ptr<FaderControl>(mixerControls->createFaderControl());
	add(gainControl);
	derive(gainControl.get());
}

MixerControls* MixControls::getMixerControls() {
	return mixerControls;
}

float& MixControls::HALF_ROOT_TWO()
{
	return HALF_ROOT_TWO_;
}
float MixControls::HALF_ROOT_TWO_ = static_cast< float >((sqrt(2) / 2));

void MixControls::derive(Control* c)
{
	switch (c->getId()) {
	case MixControlIds::MUTE:
		mute = muteControl->getValue();
		break;
	case MixControlIds::GAIN:
		gain = gainControl->getGain();
		break;
	case MixControlIds::LCR:
		left = lcrControl->getLeft();
		right = lcrControl->getRight();
		break;
	}
}


bool MixControls::isMaster()
{
    return master;
}


shared_ptr<ChannelFormat> MixControls::getChannelFormat()
{
    return busControls->getChannelFormat();
}

bool MixControls::isMute()
{
    return mute;
}

bool MixControls::isEnabled()
{
	return !isMute();
}

float MixControls::getGain()
{
    return gain;
}

void MixControls::getChannelGains(vector<float>* dest)
{
    (*dest)[1] = gain * right;
    (*dest)[0] = gain * left;
}

float MixControls::getSmoothingFactor()
{
    return mixerControls->getSmoothingFactor();
}

EnumControl* MixControls::createRouteControl(int stripId)
{
	return nullptr;
}


BooleanControl* MixControls::createMuteControl()
{
    return new BooleanControl(MixControlIds::MUTE, "Mute");
}

string MixControls::getName()
{
    return Control::getName();
}
