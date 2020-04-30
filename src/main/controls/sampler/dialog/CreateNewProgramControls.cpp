#include <controls/sampler/dialog/CreateNewProgramControls.hpp>

#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

CreateNewProgramControls::CreateNewProgramControls() 
	: AbstractSamplerControls()
{
}

void CreateNewProgramControls::function(int i)
{
	init();
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	switch (i) {
	case int(3) :
		lLs->openScreen("program");
		break;
	case 4:
		auto newProgram = lSampler->addProgram().lock();
		newProgram->setName(swGui->getNewName());
		newProgram->setMidiProgramChange(swGui->getNewProgramChange());
		mpcSoundPlayerChannel->setProgram(lSampler->getProgramCount() - 1);
		lLs->openScreen("program");
		break;
	}
}
