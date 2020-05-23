#include <controls/sampler/window/KeepOrRetryControls.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>
#include <ui/NameGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

KeepOrRetryControls::KeepOrRetryControls() 
	: AbstractSamplerControls()
{
}

void KeepOrRetryControls::mainScreen() {
	
	sampler.lock()->deleteSound(dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getPreviewSound().lock()));
	AbstractSamplerControls::mainScreen();
}

void KeepOrRetryControls::function(int i)
{
	init();
	
	switch (i) {
	case 1:
		sampler.lock()->deleteSound(dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getPreviewSound().lock()));
		ls.lock()->openScreen("sample");
		break;
	case 3 :
		if (Mpc::instance().getControls().lock()->isF4Pressed()) return;
		Mpc::instance().getControls().lock()->setF4Pressed(true);
		sampler.lock()->playPreviewSample(0, sampler.lock()->getPreviewSound().lock()->getLastFrameIndex(), 0, 2);
		break;
	case 4:
		sampler.lock()->getLastNp(program.lock().get())->setSoundNumber(sampler.lock()->getSoundCount() - 1);
		soundGui->initZones(sampler.lock()->getPreviewSound().lock()->getLastFrameIndex());
		sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		ls.lock()->openScreen("sample");
		break;
	}
}

void KeepOrRetryControls::turnWheel(int i)
{
    init();
    nameGui->setName(dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getPreviewSound().lock())->getName());
    nameGui->setParameterName(param);
    ls.lock()->openScreen("name");
}
