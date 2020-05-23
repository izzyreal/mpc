#include "DrumScreen.hpp"

#include <ui/sampler/SamplerGui.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

DrumControls::DrumControls(const int layerIndex) 
	: ScreenComponent("drum", layerIndex)
{
}

void DrumControls::function(int f)
{
    init();
	
	switch (f)
	{
    case int (0):
        ls.lock()->openScreen("programassign");
        break;
    case 1:
        ls.lock()->openScreen("programparams");
        break;
    case 2:
        ls.lock()->openScreen("selectdrum");
        break;
    case int (3):
        ls.lock()->openScreen("purge");
        break;
    }

}

void DrumControls::turnWheel(int i)
{
	init();
	
	if (param.compare("drum") == 0)
	{
		samplerGui->setSelectedDrum(samplerGui->getSelectedDrum() + i);
	}
	else if (param.compare("pgm") == 0)
	{
		mpcSoundPlayerChannel->setProgram(mpcSoundPlayerChannel->getProgram() + i);
	}
	else if (param.compare("programchange") == 0)
	{
		mpcSoundPlayerChannel->setReceivePgmChange(i > 0);
	}
	else if (param.compare("midivolume") == 0)
	{
		mpcSoundPlayerChannel->setReceiveMidiVolume(i > 0);
	}
	else if (param.compare("padtointernalsound") == 0)
	{
		samplerGui->setPadToIntSound(i > 0);
	}
}
