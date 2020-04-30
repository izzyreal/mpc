#include <controls/sampler/window/ProgramControls.hpp>

#include <Mpc.hpp>
#include <ui/NameGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

ProgramControls::ProgramControls() 
	: AbstractSamplerControls()
{
}

void ProgramControls::turnWheel(int i)
{
    init();
    	auto lLs = ls.lock();
	auto lProgram = program.lock();
	if (param.compare("programname") == 0) {
        nameGui->setName(lProgram->getName());
        nameGui->setParameterName(param);
        lLs->openScreen("name");
    }
	else if (param.compare("midiprogramchange") == 0) {
		lProgram->setMidiProgramChange(lProgram->getMidiProgramChange() + i);
	}
}

void ProgramControls::function(int i)
{
	super::function(i);
	auto lLs = ls.lock();
	int letterNumber;
	string newName;
	switch (i) {
	case 1:
		lLs->openScreen("deleteprogram");
		break;
	case 2:
		letterNumber = sampler.lock()->getProgramCount() + 21;
		newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
		swGui->setNewName(newName);
		lLs->openScreen("createnewprogram");
		break;
	case 4:
		lLs->openScreen("copyprogram");
		break;
	}
}
