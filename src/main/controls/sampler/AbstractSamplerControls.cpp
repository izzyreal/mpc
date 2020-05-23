#include "AbstractSamplerControls.hpp"

#include <controls/Controls.hpp>

#include <Mpc.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <ui/sampler/window/ZoomGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/DrumScreen.hpp>

#include <cmath>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls::sampler;
using namespace std;

AbstractSamplerControls::AbstractSamplerControls() 
	: BaseControls()
{
	auto uis = Mpc::instance().getUis().lock();
	zoomGui = uis->getZoomGui();
	soundGui = uis->getSoundGui();
	editSoundGui = uis->getEditSoundGui();
	swGui = uis->getSamplerWindowGui();
}

void AbstractSamplerControls::init()
{
	BaseControls::init();
	
	if (sampler.lock()->getSoundCount() != 0)
	{
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(sampler.lock()->getSoundIndex()).lock());
	}

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));

	mpcSoundPlayerChannel = sampler.lock()->getDrum(drumScreen->getDrum());
	program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
	auto lProgram = program.lock();
	
	if (csn.compare("programassign") == 0)
	{
		lastPad = sampler.lock()->getLastPad(lProgram.get());
		auto note = lastPad->getNote();
		lastNp = dynamic_cast<mpc::sampler::NoteParameters*>(lProgram->getNoteParameters(note));
	}
	else
	{
		lastNp = sampler.lock()->getLastNp(lProgram.get());
	}
	splittable = param.compare("st") == 0 || param.compare("end") == 0 || param.compare("to") == 0 || param.compare("endlengthvalue") == 0;
}
