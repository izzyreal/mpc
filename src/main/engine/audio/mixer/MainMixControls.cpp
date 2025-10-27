#include "engine/audio/mixer/MainMixControls.hpp"
#include "engine/control/EnumControl.hpp"
#include "engine/audio/mixer/MixerControls.hpp"
#include "engine/audio/mixer/MixerControlsIds.hpp"
#include "engine/audio/mixer/MixControlIds.hpp"
#include "engine/control/Control.hpp"

using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::control;
using namespace std;

MainMixControls::MainMixControls(MixerControls *mixerControls, int stripId,
                                 shared_ptr<BusControls> busControls,
                                 bool isMaster)
    : MixControls(mixerControls, stripId, busControls, isMaster)
{
}

EnumControl *MainMixControls::createRouteControl(int stripId)
{
    if (stripId != MixerControlsIds::MAIN_STRIP &&
        stripId != MixerControlsIds::AUX_STRIP)
    {
        auto controls = mixerControls->getControls();
        routeControl = new EnumControl(MixControlIds::ROUTE, "Route",
                                       controls[0]->getName());
        return routeControl;
    }
    return nullptr;
}

EnumControl *MainMixControls::getRouteControl()
{
    return routeControl;
}
