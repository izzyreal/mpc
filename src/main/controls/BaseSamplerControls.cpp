#include "BaseSamplerControls.hpp"

#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/DrumScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace std;

BaseSamplerControls::BaseSamplerControls(mpc::Mpc& mpc)
	: BaseControls(mpc)
{
}

void BaseSamplerControls::init()
{
	BaseControls::init();
	
	// Screens that extend this class need the below overrides
	auto drumScreen = mpc.screens->get<DrumScreen>("drum");
	mpcSoundPlayerChannel = sampler.lock()->getDrum(drumScreen->drum);
    program = sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram());
	
	splittable = param.compare("st") == 0 || param.compare("end") == 0 || param.compare("to") == 0 || param.compare("endlengthvalue") == 0 || param.compare("start") == 0;
}
