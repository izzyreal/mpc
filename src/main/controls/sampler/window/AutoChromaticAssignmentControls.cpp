#include <controls/sampler/window/AutoChromaticAssignmentControls.hpp>

#include <ui/NameGui.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

AutoChromaticAssignmentControls::AutoChromaticAssignmentControls() 
	: AbstractSamplerControls()
{
}

void AutoChromaticAssignmentControls::turnWheel(int i)
{
	init();
	
	auto lLs = ls.lock();
	auto lProgram = program.lock();
	if (param.compare("source") == 0) {
		mpc.setPadAndNote(mpc.getPad(), mpc.getNote() + i);
		swGui->setAutoChromAssSnd(sampler.lock()->getLastNp(lProgram.get())->getSndNumber(), sampler.lock()->getSoundCount());
	}
	else if (param.compare("programname") == 0) {
		nameGui->setName(swGui->getNewName());
		nameGui->setParameterName("autochrom");
		lLs->openScreen("name");
	}
	else if (param.compare("snd") == 0) {
		swGui->setAutoChromAssSnd(swGui->getAutoChromAssSnd() + i, sampler.lock()->getSoundCount());
	}
	else if (param.compare("originalkey") == 0) {
		swGui->setOriginalKey(swGui->getOriginalKey() + i);
	}
	else if (param.compare("tune") == 0) {
		swGui->setTune(swGui->getTune() + i);
	}
}