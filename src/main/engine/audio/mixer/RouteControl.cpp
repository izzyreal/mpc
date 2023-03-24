#include <engine/audio/mixer/RouteControl.hpp>
#include <engine/audio/mixer/MainMixControls.hpp>
#include <engine/audio/mixer/MixControlIds.hpp>
#include <engine/control/Control.hpp>

using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::control;
using namespace std;

RouteControl::RouteControl(MainMixControls* mmc, string defaultRoute, bool canRouteToGroups) : EnumControl(MixControlIds::ROUTE, string("Route"), defaultRoute)
{
	this->mmc = mmc;
	canRouteToGroups = true;
	this->canRouteToGroups = canRouteToGroups;
}


