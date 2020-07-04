#include "ApsLoader.hpp"

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/SoundLoader.hpp>
#include <file/aps/ApsAssignTable.hpp>
#include <file/aps/ApsDrumConfiguration.hpp>
#include <file/aps/ApsGlobalParameters.hpp>
#include <file/aps/ApsMixer.hpp>
#include <file/aps/ApsNoteParameters.hpp>
#include <file/aps/ApsParser.hpp>
#include <file/aps/ApsProgram.hpp>
#include <file/aps/ApsSlider.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/PgmSlider.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::disk;
using namespace mpc::file::aps;
using namespace moduru::lang;
using namespace std;

ApsLoader::ApsLoader(mpc::disk::MpcFile* file) 
{
	
	this->file = file;
	
	if (loadThread.joinable())
	{
		loadThread.join();
	}

	loadThread = thread(&ApsLoader::static_load, this);
}

void ApsLoader::static_load(void* this_p)
{
	static_cast<ApsLoader*>(this_p)->load();
}

void ApsLoader::load()
{
	auto disk = Mpc::instance().getDisk().lock();
	disk->setBusy(true);
	ApsParser apsParser(file);

	if (!apsParser.isHeaderValid())
	{
		MLOG("The APS file you're trying to load does not have a valid ID. The first 2 bytes of an MPC2000XL APS file should be 0A 05. MPC2000 APS files start with 0A 04 and are not supported (yet?).");
		disk->setBusy(false);
		return;
	}

	auto sampler = Mpc::instance().getSampler().lock();
	sampler->deleteAllSamples();
	const bool initPgms = false;

	for (int i = 0; i < apsParser.getSoundNames().size(); i++)
	{
		auto ext = "snd";
		mpc::disk::MpcFile* soundFile = nullptr;
		string soundFileName = StrUtil::replaceAll(apsParser.getSoundNames()[i], ' ', "");

		for (auto& f : disk->getAllFiles())
		{
			if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".SND"))
			{
				soundFile = f;
			}
		}

		if (soundFile == nullptr)
		{
			for (auto& f : disk->getAllFiles())
			{
				if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".WAV"))
				{
					soundFile = f;
				}
			}

			ext = "wav";
		}

		if (soundFile == nullptr)
		{
			MLOG("Error loading sound " + soundFileName + ". Check if such a file with SND or WAV extension exists next to your APS or PGM file.");
			continue;
		}

		loadSound(soundFileName, ext, soundFile, false, i);
	}

	sampler->deleteAllPrograms(initPgms);
	
	for (auto& p : apsParser.getPrograms())
	{
		auto newProgram = sampler->addProgram(p->index).lock();
		auto assignTable = p->getAssignTable()->get();
		newProgram->setName(p->getName());

		for (int i = 0; i < 64; i++)
		{
			int padnn = assignTable[i];
			newProgram->getPad(i)->setNote(padnn);
			auto apssmc = p->getStereoMixerChannel(i + 35);
			auto apsifmc = p->getIndivFxMixerChannel(i + 35);
			auto smc = newProgram->getPad(i)->getStereoMixerChannel().lock();
			auto ifmc = newProgram->getPad(i)->getIndivFxMixerChannel().lock();
			ifmc->setFxPath(apsifmc->getFxPath());
			smc->setLevel(apssmc->getLevel());
			smc->setPanning(apssmc->getPanning());
			ifmc->setVolumeIndividualOut(apsifmc->getVolumeIndividualOut());
			ifmc->setFxSendLevel(apsifmc->getFxSendLevel());
			ifmc->setOutput(apsifmc->getOutput());
			auto np = dynamic_cast<mpc::sampler::NoteParameters*>(newProgram->getNoteParameters(i + 35));
			auto anp = p->getNoteParameters(i + 35);
			np->setSoundNumber(anp->getSoundNumber());
			np->setTune(anp->getTune());
			np->setVoiceOverlap(anp->getVoiceOverlap());
			np->setDecayMode(anp->getDecayMode());
			np->setAttack(anp->getAttack());
			np->setDecay(anp->getDecay());
			np->setFilterAttack(anp->getVelocityToFilterAttack());
			np->setFilterDecay(anp->getVelocityToFilterDecay());
			np->setFilterEnvelopeAmount(anp->getVelocityToFilterAmount());
			np->setFilterFrequency(anp->getCutoffFrequency());
			np->setFilterResonance(anp->getResonance());
			np->setMuteAssignA(anp->getMute1());
			np->setMuteAssignB(anp->getMute2());
			np->setOptNoteA(anp->getAlsoPlay1());
			np->setOptionalNoteB(anp->getAlsoPlay2());
			np->setSliderParameterNumber(anp->getSliderParameter());
			np->setSoundGenMode(anp->getSoundGenerationMode());
			np->setVelocityToStart(anp->getVelocityToStart());
			np->setVelocityToAttack(anp->getVelocityToAttack());
			np->setVelocityToFilterFrequency(anp->getVelocityToFilterFrequency());
			np->setVeloToLevel(anp->getVelocityToLevel());
			np->setVeloRangeLower(anp->getVelocityRangeLower());
			np->setVeloRangeUpper(anp->getVelocityRangeUpper());
			np->setVelocityToPitch(anp->getVelocityToPitch());
		}

		auto slider = dynamic_cast<mpc::sampler::PgmSlider*>(newProgram->getSlider());
		slider->setAttackHighRange(p->getSlider()->getAttackHigh());
		slider->setAttackLowRange(p->getSlider()->getAttackLow());
		slider->setControlChange(p->getSlider()->getProgramChange());
		slider->setDecayHighRange(p->getSlider()->getDecayHigh());
		slider->setDecayLowRange(p->getSlider()->getDecayLow());
		slider->setFilterHighRange(p->getSlider()->getFilterHigh());
		slider->setFilterLowRange(p->getSlider()->getFilterLow());
		slider->setAssignNote(p->getSlider()->getNote());
		slider->setTuneHighRange(p->getSlider()->getTuneHigh());
		slider->setTuneLowRange(p->getSlider()->getTuneLow());
	}

	for (int i = 0; i < 4; i++)
	{
		auto m = apsParser.getDrumMixers()[i];
		auto drum = Mpc::instance().getDrum(i);

		for (int note = 35; note <= 98; note++)
		{
			auto apssmc = m->getStereoMixerChannel(note);
			auto apsifmc = m->getIndivFxMixerChannel(note);
			auto drumsmc = drum->getStereoMixerChannels()[note - 35].lock();
			auto drumifmc = drum->getIndivFxMixerChannels()[note - 35].lock();
			drumifmc->setFxPath(apsifmc->getFxPath());
			drumsmc->setLevel(apssmc->getLevel());
			drumsmc->setPanning(apssmc->getPanning());
			drumifmc->setVolumeIndividualOut(apsifmc->getVolumeIndividualOut());
			drumifmc->setOutput(apsifmc->getOutput());
			drumifmc->setFxSendLevel(apsifmc->getFxSendLevel());
		}

		auto pgm = apsParser.getDrumConfiguration(i)->getProgram();
		drum->setProgram(pgm);
		drum->setReceivePgmChange(apsParser.getDrumConfiguration(i)->getReceivePgmChange());
		drum->setReceiveMidiVolume(apsParser.getDrumConfiguration(i)->getReceiveMidiVolume());
	}

	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));

	mixerSetupScreen->setRecordMixChangesEnabled(apsParser.getGlobalParameters()->isRecordMixChangesEnabled());
	mixerSetupScreen->setCopyPgmMixToDrumEnabled(apsParser.getGlobalParameters()->isCopyPgmMixToDrumEnabled());
	mixerSetupScreen->setFxDrum(apsParser.getGlobalParameters()->getFxDrum());
	mixerSetupScreen->setIndivFxSourceDrum(apsParser.getGlobalParameters()->isIndivFxSourceDrum());
	mixerSetupScreen->setStereoMixSourceDrum(apsParser.getGlobalParameters()->isStereoMixSourceDrum());

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));

	drumScreen->setPadToIntSound(apsParser.getGlobalParameters()->isPadToIntSoundEnabled());
	
	sampler->setSoundIndex(0);
	
	Mpc::instance().getLayeredScreen().lock()->openScreen("load");
	disk->setBusy(false);
}

void ApsLoader::loadSound(string soundFileName, string ext, mpc::disk::MpcFile* soundFile, bool replace, int loadSoundIndex)
{
	auto sl = mpc::disk::SoundLoader(Mpc::instance().getSampler().lock()->getSounds(), replace);
	sl.setPartOfProgram(true);
	showPopup(soundFileName, ext, soundFile->length());
	sl.loadSound(soundFile);
}

void ApsLoader::showPopup(string name, string ext, int sampleSize)
{
	Mpc::instance().getLayeredScreen().lock()->openScreen("popup");
	auto popupScreen = dynamic_pointer_cast<PopupScreen>(Screens::getScreenComponent("popup"));
	popupScreen->setText("LOADING " + StrUtil::padRight(name, " ", 16) + "." + ext);

	if (dynamic_pointer_cast<mpc::disk::StdDisk>(Mpc::instance().getDisk().lock()))
	{
		auto sleepTime = sampleSize / 800;
	
		if (sleepTime < 300)
		{
			sleepTime = 300;
		}
		this_thread::sleep_for(chrono::milliseconds((int)(sleepTime * 0.2)));
	}
}

ApsLoader::~ApsLoader()
{
	if (loadThread.joinable())
	{
		loadThread.join();
	}
}
