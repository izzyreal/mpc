#include <controls/sampler/window/NumberOfZonesControls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

NumberOfZonesControls::NumberOfZonesControls() 
	: AbstractSamplerControls()
{
}

void NumberOfZonesControls::function(int f)
{
    super::function(f);
    switch (f) {
    case 4:
        soundGui->initZones(sound.lock()->getLastFrameIndex());
        ls.lock()->openScreen("zone");
        break;
    }
}

void NumberOfZonesControls::turnWheel(int i)
{
    init();
	if (param.compare("") == 0) {
		return;
	}
	if (param.compare("numberofzones") == 0) {
		soundGui->setNumberOfZones(soundGui->getNumberOfZones() + i);
	}
}
