#include <engine/audio/mixer/MainMixControls.hpp>
#include <engine/audio/mixer/RouteControl.hpp>
#include <engine/audio/mixer/MixerControls.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>
#include <engine/control/Control.hpp>
#include <engine/control/EnumControl.hpp>

using namespace ctoot::audio::mixer;
using namespace std;

MainMixControls::MainMixControls(MixerControls* mixerControls, int stripId, shared_ptr<BusControls> busControls, bool isMaster)
	: MixControls(mixerControls, stripId, busControls, isMaster)
{
}

ctoot::control::EnumControl* MainMixControls::createRouteControl(int stripId)
{
	if (stripId != MixerControlsIds::MAIN_STRIP && stripId != MixerControlsIds::AUX_STRIP) {
		auto controls = mixerControls->getControls();
		auto c = controls[0];
		routeControl = new RouteControl(this, c->getName(), stripId == MixerControlsIds::CHANNEL_STRIP);
		return routeControl;
	}
	return nullptr;
}

ctoot::control::EnumControl* MainMixControls::getRouteControl()
{
    return routeControl;
}

MainMixControls::~MainMixControls() {
}
