#include "BaseSamplerControls.hpp"

#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/DrumScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace std;

void BaseSamplerControls::init()
{
	BaseControls::init();
	
	// Screens that extend this class need the below overrides
	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));
	mpcSoundPlayerChannel = sampler.lock()->getDrum(drumScreen->getDrum());
	program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	
	splittable = param.compare("st") == 0 || param.compare("end") == 0 || param.compare("to") == 0 || param.compare("endlengthvalue") == 0;
}
