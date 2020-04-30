#include <controls/sampler/DrumControls.hpp>

#include <ui/sampler/SamplerGui.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::controls::sampler;
using namespace std;

DrumControls::DrumControls() 
	: AbstractSamplerControls()
{
}

void DrumControls::function(int f)
{
    init();
	auto lLs = ls.lock();
	switch (f) {
    case int (0):
        lLs->openScreen("programassign");
        break;
    case 1:
        lLs->openScreen("programparams");
        break;
    case 2:
        lLs->openScreen("selectdrum");
        break;
    case int (3):
        lLs->openScreen("purge");
        break;
    }

}

void DrumControls::turnWheel(int i)
{
	init();
	auto yes = i > 0;
	if (param.compare("drum") == 0) {
		samplerGui->setSelectedDrum(samplerGui->getSelectedDrum() + i);
	}
	else if (param.compare("pgm") == 0) {
		mpcSoundPlayerChannel->setProgram(mpcSoundPlayerChannel->getProgram() + i);
	}
	else if (param.compare("programchange") == 0) {
		mpcSoundPlayerChannel->setReceivePgmChange(yes);
	}
	else if (param.compare("midivolume") == 0) {
		mpcSoundPlayerChannel->setReceiveMidiVolume(yes);
	}
	else if (param.compare("padtointernalsound") == 0) {
		samplerGui->setPadToIntSound(yes);
	}
}
