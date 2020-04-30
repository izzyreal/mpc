#include <controls/sampler/window/CopyNoteParametersControls.hpp>

#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

CopyNoteParametersControls::CopyNoteParametersControls()
	: AbstractSamplerControls()
{
}

void CopyNoteParametersControls::turnWheel(int i)
{
    init();
    const int pCount = sampler.lock()->getProgramCount();
	if (param.compare("prog0") == 0) {
		swGui->setProg0(swGui->getProg0() + i, pCount);
	}
	else if (param.compare("note0") == 0) {
		swGui->setNote0(swGui->getNote0() + i);
	}
	else if (param.compare("prog1") == 0) {
		swGui->setProg1(swGui->getProg1() + i, pCount);
	}
	else if (param.compare("note1") == 0) {
		swGui->setNote1(swGui->getNote1() + i);
	}
}

void CopyNoteParametersControls::function(int i)
{
	super::function(i);
	mpc::sampler::NoteParameters* source;
	mpc::sampler::Program* dest;
	mpc::sampler::NoteParameters* clone;
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	switch (i) {
	case 4:
		source = dynamic_cast<mpc::sampler::NoteParameters*>(lSampler->getProgram(swGui->getProg0()).lock()->getNoteParameters(swGui->getNote0()));
		dest = dynamic_cast<mpc::sampler::Program*>(lSampler->getProgram(swGui->getProg1()).lock().get());
		clone = source->clone();
		dest->setNoteParameters(swGui->getNote1(), clone);
		lLs->openScreen("programassign");
		break;
	}
}
