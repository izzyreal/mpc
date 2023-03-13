#include <engine/audio/mixer/RouteControl.hpp>
#include <engine/audio/core/AudioControlsChain.hpp>
#include <engine/audio/mixer/MainMixControls.hpp>
#include <engine/audio/mixer/MixControlIds.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>
#include <engine/control/Control.hpp>

using namespace ctoot::audio::mixer;
using namespace ctoot::control;
using namespace std;

RouteControl::RouteControl(MainMixControls* mmc, string defaultRoute, bool canRouteToGroups) : EnumControl(MixControlIds::ROUTE, string("Route"), defaultRoute)
{
	this->mmc = mmc;
	canRouteToGroups = true;
	this->canRouteToGroups = canRouteToGroups;
	indicator = !canRouteToGroups;
}


vector<nonstd::any> RouteControl::getValues()
{
	vector<nonstd::any> values;
	auto controls = mmc->getMixerControls()->getControls();
	for (auto& c : controls) {
		auto control = c;
		if (dynamic_pointer_cast<core::AudioControlsChain>(control)) {
			if (control->getId() == MixerControlsIds::MAIN_STRIP || (control->getId() == MixerControlsIds::GROUP_STRIP && canRouteToGroups)) {
				values.push_back(control->getName());
			}
		}
	}
	return values;
}
