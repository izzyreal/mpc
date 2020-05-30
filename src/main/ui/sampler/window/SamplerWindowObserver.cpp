#include "SamplerWindowObserver.hpp"

#include <Mpc.hpp>

#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/EnvGraph.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/Layer.hpp>

#include <ui/sampler/window/SamplerWindowGui.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>

#include <lang/StrUtil.hpp>

#include <cmath>

using namespace mpc::ui::sampler::window;
using namespace std;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SamplerWindowObserver::SamplerWindowObserver()
{	
	Mpc::instance().addObserver(this);

	auto uis = Mpc::instance().getUis().lock();
	swGui = uis->getSamplerWindowGui();
	swGui->addObserver(this);
	sampler = Mpc::instance().getSampler();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	csn = ls->getCurrentScreenName();
	
	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));

	mpcSoundPlayerChannel = sampler.lock()->getDrum(drumScreen->getDrum());

	program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	auto lProgram = program.lock();
	sampler.lock()->getLastNp(lProgram.get())->addObserver(this);
	sampler.lock()->getLastPad(lProgram.get())->addObserver(this);
	lProgram->addObserver(this);
	mpcSoundPlayerChannel->addObserver(this);

	if (csn.compare("deleteprogram") == 0) {
		pgmField = ls->lookupField("pgm");
		displayPgm();
	}
	else if (csn.compare("createnewprogram") == 0) {
		newNameField = ls->lookupField("newname");
		midiProgramChangeField = ls->lookupField("midiprogramchange");

		auto letterNumber = sampler.lock()->getProgramCount() + 21;
		auto newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
		//setNewName(newName);

		//displayNewName();
		//displayMidiProgramChange();
	}
	else if (csn.compare("copyprogram") == 0) {
		pgm0Field = ls->lookupField("pgm0");
		pgm1Field = ls->lookupField("pgm1");
		displayPgm0();
		displayPgm1();
	}
}

void SamplerWindowObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	auto lProgram = program.lock();

	sampler.lock()->getLastNp(lProgram.get())->deleteObserver(this);
	sampler.lock()->getLastPad(lProgram.get())->deleteObserver(this);
	lProgram->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));
	mpcSoundPlayerChannel = sampler.lock()->getDrum(drumScreen->getDrum());

	program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock());

	sampler.lock()->getLastNp(lProgram.get())->addObserver(this);
	sampler.lock()->getLastPad(lProgram.get())->addObserver(this);
	lProgram->addObserver(this);
	mpcSoundPlayerChannel->addObserver(this);

	string s = nonstd::any_cast<string>(arg);

	if (s.compare("newprogramchange") == 0 || s.compare("midiprogramchange") == 0) {
		//displayMidiProgramChange();
	}
	else if (s.compare("deletepgm") == 0) {
		displayPgm();
	}
	else if (s.compare("pgm0") == 0) {
		displayPgm0();
	}
	else if (s.compare("pgm1") == 0) {
		displayPgm1();
	}
}

void SamplerWindowObserver::displayPgm0()
{
	pgm0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getPgm0() + 1), " ", 2) + "-" + dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(swGui->getPgm0()).lock())->getName());
}

void SamplerWindowObserver::displayPgm1()
{
	pgm1Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getPgm1() + 1), " ", 2) + "-" + dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(swGui->getPgm1()).lock())->getName());
}

void SamplerWindowObserver::displayNewName()
{
	//newNameField.lock()->setText(swGui->getNewName());
}

void SamplerWindowObserver::displayPgm()
{
	pgmField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getDeletePgm() + 1), " ", 2) + "-" + dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(swGui->getDeletePgm()).lock())->getName());
}

SamplerWindowObserver::~SamplerWindowObserver() {
	if (Mpc::instance().getLayeredScreen().lock())
	{
		Mpc::instance().getLayeredScreen().lock()->getEnvGraph().lock()->Hide(true);
	}
	swGui->deleteObserver(this);
	program.lock()->deleteObserver(this);
	sampler.lock()->getLastNp(program.lock().get())->deleteObserver(this);
	sampler.lock()->getLastPad(program.lock().get())->deleteObserver(this);
	mpcSoundPlayerChannel->deleteObserver(this);
}
