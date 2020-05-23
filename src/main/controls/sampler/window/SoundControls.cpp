#include <controls/sampler/window/SoundControls.hpp>

#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

SoundControls::SoundControls() 
	: AbstractSamplerControls()
{
}

void SoundControls::turnWheel(int i)
{
	init();
	
	if (param.compare("soundname") == 0)
	{
		nameGui->setName(ls.lock()->lookupField("soundname").lock()->getText());
		nameGui->setParameterName("soundname");
		ls.lock()->openScreen("name");
	}
}

void SoundControls::function(int i)
{
	BaseControls::function(i);

	switch (i)
	{
	case 1:
		ls.lock()->openScreen("deletesound");
		break;
	case 2:
		ls.lock()->openScreen("convertsound");
		break;
	case 4:
		auto newSampleName = sampler.lock()->getSound().lock()->getName();
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		ls.lock()->openScreen("copysound");
		soundGui->setNewName(newSampleName);
		break;
	}
}
