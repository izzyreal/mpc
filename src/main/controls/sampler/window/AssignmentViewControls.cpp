#include <controls/sampler/window/AssignmentViewControls.hpp>

#include <lcdgui/Field.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

AssignmentViewControls::AssignmentViewControls() 
	: AbstractSamplerControls()
{
}

void AssignmentViewControls::up()
{
	init();
	if (param.find("0") != string::npos) return;

	auto nn = program.lock()->getPad(samplerGui->getPad() + 4)->getNote();
	auto focusPadNr = samplerGui->getPad() + 4;
	while (focusPadNr > 15) focusPadNr -= 16;

	ls.lock()->setFocus(mpc::ui::sampler::window::SamplerWindowGui::padFocusNames[focusPadNr]);
	samplerGui->setPadAndNote(samplerGui->getPad() + 4, nn);
}

void AssignmentViewControls::down()
{
	init();
	if (param.find("3") != string::npos) return;

	auto nn = program.lock()->getPad(samplerGui->getPad() - 4)->getNote();
	auto focusPadNr = samplerGui->getPad() - 4;
	while (focusPadNr > 15) focusPadNr -= 16;

	ls.lock()->setFocus(mpc::ui::sampler::window::SamplerWindowGui::padFocusNames[focusPadNr]);
	samplerGui->setPadAndNote(samplerGui->getPad() - 4, nn);
}

void AssignmentViewControls::left()
{
    init();
    if(param.find("a") != string::npos) return;
    super::left();
    auto padNr = samplerGui->getPad() - 1;
    auto nn = program.lock()->getPad(padNr)->getNote();
    samplerGui->setPadAndNote(padNr, nn);
}

void AssignmentViewControls::right()
{
	init();
	if (param.find("d") != string::npos) return;

	super::right();
	auto padNr = samplerGui->getPad() + 1;
	auto nn = program.lock()->getPad(padNr)->getNote();
	samplerGui->setPadAndNote(padNr, nn);
}

void AssignmentViewControls::turnWheel(int i)
{
	init();
	
	auto lProgram = program.lock();
	sampler.lock()->getLastPad(lProgram.get())->setNote(sampler.lock()->getLastPad(lProgram.get())->getNote() + i);
}
