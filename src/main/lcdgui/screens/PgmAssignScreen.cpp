#include <lcdgui/screens/PgmAssignScreen.hpp>

#include <Mpc.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

PgmAssignScreen::PgmAssignScreen(const int layerIndex)
	: ScreenComponent("programassign", layerIndex)
{
}

void PgmAssignScreen::function(int i)
{
	init();
		
	switch (i)
	{
	case 0:
		ls.lock()->openScreen("selectdrum");
		break;
	case 1:
		ls.lock()->openScreen("programparams");
		break;
	case 2:
		ls.lock()->openScreen("drum");
		break;
	case 3:
		ls.lock()->openScreen("purge");
		break;
	case 4:
	{
		auto samplerWindowGui = mpc.getUis().lock()->getSamplerWindowGui();
		samplerWindowGui->setAutoChromAssSnd(sampler.lock()->getLastNp(program.lock().get())->getSndNumber(), sampler.lock()->getSoundCount());

		auto letterNumber = sampler.lock()->getProgramCount() + 21;
		auto newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
		
		samplerWindowGui->setNewName(newName);
		samplerGui->setPrevScreenName(csn);
		ls.lock()->openScreen("autochromaticassignment");
		break;
	}
	}
}

void PgmAssignScreen::turnWheel(int i)
{
	init();

	auto lastPad = sampler.lock()->getLastPad(program.lock().get());
	auto lastNoteParameters = dynamic_cast<mpc::sampler::NoteParameters*>(program.lock()->getNoteParameters(lastPad->getNote()));

	if (param.compare("padassign") == 0)
	{
		samplerGui->setPadAssignMaster(i > 0);
	}
	else if (param.compare("pgm") == 0)
	{
		auto pgm = mpcSoundPlayerChannel->getProgram();
		auto candidate = sampler.lock()->getUsedProgram(pgm, i > 0);

		if (candidate != pgm)
		{
			mpcSoundPlayerChannel->setProgram(candidate);
		}
	}
	else if (param.compare("pad") == 0)
	{
		auto candidate = samplerGui->getPad() + i;
		
		if (candidate < 0 || candidate > 63)
		{
			return;
		}

		auto nextNote = program.lock()->getPad(candidate)->getNote();
		samplerGui->setPadAndNote(candidate, nextNote);
	}
	else if (param.compare("padnote") == 0)
	{
		lastPad->setNote(lastPad->getNote() + i);

		auto candidate = lastPad->getNote();
		
		if (candidate > 34)
		{
			samplerGui->setPadAndNote(samplerGui->getPad(), candidate);
		}
	}
	else if (param.compare("note") == 0)
	{
		auto candidate = samplerGui->getNote() + i;
	
		if (candidate > 34)
		{
			samplerGui->setPadAndNote(samplerGui->getPad(), candidate);
		}

	}
	else if (param.compare("snd") == 0)
 {		
		lastNoteParameters->setSoundNumber(sampler.lock()->getNextSoundIndex(lastNoteParameters->getSndNumber(), i > 0));
		auto sound = sampler.lock()->getSound(lastNoteParameters->getSndNumber()).lock();
		
		if (sound)
		{
			if (sound->isMono())
			{
				lastPad->getStereoMixerChannel().lock()->setStereo(false);
			}
			
			if (sound->isLoopEnabled())
			{
				lastNoteParameters->setVoiceOverlap(2);
			}
		}
	}
	else if (param.compare("mode") == 0)
	{
		lastNoteParameters->setSoundGenMode(lastNoteParameters->getSoundGenerationMode() + i);
	}
	else if (param.compare("velocityrangelower") == 0)
	{
		lastNoteParameters->setVeloRangeLower(lastNoteParameters->getVelocityRangeLower() + i);
	}
	else if (param.compare("velocityrangeupper") == 0)
	{
		lastNoteParameters->setVeloRangeUpper(lastNoteParameters->getVelocityRangeUpper() + i);
	}
	else if (param.compare("optionalnotenumbera") == 0)
	{
		lastNoteParameters->setOptNoteA(lastNoteParameters->getOptionalNoteA() + i);
	}
	else if (param.compare("optionalnotenumberb") == 0)
	{
		lastNoteParameters->setOptionalNoteB(lastNoteParameters->getOptionalNoteB() + i);
	}
}

void PgmAssignScreen::openWindow()
{
	init();
	
	if (param.compare("pgm") == 0)
	{
		samplerGui->setPrevScreenName("programassign");
		ls.lock()->openScreen("program");
	}
	else if (param.compare("pad") == 0 || param.compare("padnote") == 0)
	{
		ls.lock()->openScreen("assignmentview");
	}
	else if (param.compare("padassign") == 0)
	{
		ls.lock()->openScreen("initpadassign");
	}
	else if (param.compare("note") == 0)
	{
		auto pn = mpcSoundPlayerChannel->getProgram();
		auto nn = samplerGui->getNote();
		auto pc = sampler.lock()->getProgramCount();
		
		auto samplerWindowGui = mpc.getUis().lock()->getSamplerWindowGui();
		samplerWindowGui->setProg0(pn, pc);
		samplerWindowGui->setNote0(nn);
		samplerWindowGui->setProg1(pn, pc);
		samplerWindowGui->setNote1(nn);
	
		ls.lock()->openScreen("copynoteparameters");
	}
	else if (param.compare("snd") == 0)
	{
		auto sn = sampler.lock()->getLastNp(program.lock().get())->getSndNumber();
		if (sn != -1)
		{
			sampler.lock()->setSoundIndex(sn);
			sampler.lock()->setPreviousScreenName("programassign");
			ls.lock()->openScreen("sound");
		}
	}
}
