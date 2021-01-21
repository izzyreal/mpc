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
#include <lcdgui/screens/window/CantFindFileScreen.hpp>

#include <lang/StrUtil.hpp>

#include <stdexcept>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::disk;
using namespace mpc::sampler;
using namespace mpc::file::aps;
using namespace moduru::lang;
using namespace std;

ApsLoader::ApsLoader(mpc::Mpc& mpc, mpc::disk::MpcFile* file)
	: mpc(mpc)
{
	if (!file->getFsNode().lock()->exists())
		throw invalid_argument("File does not exist");

	this->file = file;

	auto cantFindFileScreen = dynamic_pointer_cast<CantFindFileScreen>(mpc.screens->getScreenComponent("cant-find-file"));
	cantFindFileScreen->skipAll = false;

	loadThread = thread(&ApsLoader::static_load, this);
}

void ApsLoader::static_load(void* this_p)
{
	static_cast<ApsLoader*>(this_p)->load();
}

void ApsLoader::notFound(string soundFileName, string ext)
{
	auto cantFindFileScreen = dynamic_pointer_cast<CantFindFileScreen>(mpc.screens->getScreenComponent("cant-find-file"));
	auto skipAll = cantFindFileScreen->skipAll;

	if (!skipAll)
	{
		cantFindFileScreen->waitingForUser = true;

		cantFindFileScreen->fileName = soundFileName;

		mpc.getLayeredScreen().lock()->openScreen("cant-find-file");

		while (cantFindFileScreen->waitingForUser)
		{
			this_thread::sleep_for(chrono::milliseconds(25));
		}
	}
}

void ApsLoader::load()
{
	auto disk = mpc.getDisk().lock();
	disk->setBusy(true);
	ApsParser apsParser(mpc, file);

	if (!apsParser.isHeaderValid())
	{
		MLOG("The APS file you're trying to load does not have a valid ID. The first 2 bytes of an MPC2000XL APS file should be 0A 05. MPC2000 APS files start with 0A 04 and are not supported (yet?).");
		disk->setBusy(false);
		return;
	}

	auto sampler = mpc.getSampler().lock();
	sampler->deleteAllSamples();
	const bool initPgms = false;

	vector<int> unavailableSoundIndices;

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
				break;
			}
		}

		if (soundFile == nullptr || !soundFile->getFsNode().lock()->exists())
		{
			for (auto& f : disk->getAllFiles())
			{
				if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".WAV"))
				{
					soundFile = f;
					ext = "wav";
					break;
				}
			}
		}

		if (soundFile == nullptr || !soundFile->getFsNode().lock()->exists())
		{
			unavailableSoundIndices.push_back(i);
			notFound(soundFileName, ext);
			continue;
		}

		loadSound(soundFileName, ext, soundFile, false, i);
	}

	sampler->deleteAllPrograms(initPgms);
	
	for (auto& apsProgram : apsParser.getPrograms())
	{
		auto newProgram = sampler->addProgram(apsProgram->index).lock();
		auto assignTable = apsProgram->getAssignTable()->get();

		newProgram->setName(apsProgram->getName());

		for (int noteIndex = 0; noteIndex < 64; noteIndex++)
		{
			int padnn = assignTable[noteIndex];
			newProgram->getPad(noteIndex)->setNote(assignTable[noteIndex]);

			auto sourceStereoMixerChannel = apsProgram->getStereoMixerChannel(noteIndex);
			auto sourceIndivFxMixerChannel = apsProgram->getIndivFxMixerChannel(noteIndex);

			auto destNoteParams = dynamic_cast<NoteParameters*>(newProgram->getNoteParameters(noteIndex + 35));
			auto destStereoMixerCh = destNoteParams->getStereoMixerChannel().lock();
			auto destIndivFxCh = destNoteParams->getIndivFxMixerChannel().lock();

			destIndivFxCh->setFxPath(sourceIndivFxMixerChannel->getFxPath());
			destStereoMixerCh->setLevel(sourceStereoMixerChannel->getLevel());
			destStereoMixerCh->setPanning(sourceStereoMixerChannel->getPanning());
			destIndivFxCh->setVolumeIndividualOut(sourceIndivFxMixerChannel->getVolumeIndividualOut());
			destIndivFxCh->setFxSendLevel(sourceIndivFxMixerChannel->getFxSendLevel());
			destIndivFxCh->setOutput(sourceIndivFxMixerChannel->getOutput());

			auto srcNoteParams = apsProgram->getNoteParameters(noteIndex);

			auto soundIndex = srcNoteParams->getSoundNumber();

			if (find(begin(unavailableSoundIndices), end(unavailableSoundIndices), soundIndex) != end(unavailableSoundIndices))
				soundIndex = -1;

			destNoteParams->setSoundIndex(soundIndex);
			destNoteParams->setTune(srcNoteParams->getTune());
			destNoteParams->setVoiceOverlap(srcNoteParams->getVoiceOverlap());
			destNoteParams->setDecayMode(srcNoteParams->getDecayMode());
			destNoteParams->setAttack(srcNoteParams->getAttack());
			destNoteParams->setDecay(srcNoteParams->getDecay());
			destNoteParams->setFilterAttack(srcNoteParams->getVelocityToFilterAttack());
			destNoteParams->setFilterDecay(srcNoteParams->getVelocityToFilterDecay());
			destNoteParams->setFilterEnvelopeAmount(srcNoteParams->getVelocityToFilterAmount());
			destNoteParams->setFilterFrequency(srcNoteParams->getCutoffFrequency());
			destNoteParams->setFilterResonance(srcNoteParams->getResonance());
			destNoteParams->setMuteAssignA(srcNoteParams->getMute1());
			destNoteParams->setMuteAssignB(srcNoteParams->getMute2());
			destNoteParams->setOptNoteA(srcNoteParams->getAlsoPlay1());
			destNoteParams->setOptionalNoteB(srcNoteParams->getAlsoPlay2());
			destNoteParams->setSliderParameterNumber(srcNoteParams->getSliderParameter());
			destNoteParams->setSoundGenMode(srcNoteParams->getSoundGenerationMode());
			destNoteParams->setVelocityToStart(srcNoteParams->getVelocityToStart());
			destNoteParams->setVelocityToAttack(srcNoteParams->getVelocityToAttack());
			destNoteParams->setVelocityToFilterFrequency(srcNoteParams->getVelocityToFilterFrequency());
			destNoteParams->setVeloToLevel(srcNoteParams->getVelocityToLevel());
			destNoteParams->setVeloRangeLower(srcNoteParams->getVelocityRangeLower());
			destNoteParams->setVeloRangeUpper(srcNoteParams->getVelocityRangeUpper());
			destNoteParams->setVelocityToPitch(srcNoteParams->getVelocityToPitch());
		}

		auto slider = dynamic_cast<mpc::sampler::PgmSlider*>(newProgram->getSlider());
		slider->setAttackHighRange(apsProgram->getSlider()->getAttackHigh());
		slider->setAttackLowRange(apsProgram->getSlider()->getAttackLow());
		slider->setControlChange(apsProgram->getSlider()->getProgramChange());
		slider->setDecayHighRange(apsProgram->getSlider()->getDecayHigh());
		slider->setDecayLowRange(apsProgram->getSlider()->getDecayLow());
		slider->setFilterHighRange(apsProgram->getSlider()->getFilterHigh());
		slider->setFilterLowRange(apsProgram->getSlider()->getFilterLow());
		slider->setAssignNote(apsProgram->getSlider()->getNote());
		slider->setTuneHighRange(apsProgram->getSlider()->getTuneHigh());
		slider->setTuneLowRange(apsProgram->getSlider()->getTuneLow());
	}

	for (int i = 0; i < 4; i++)
	{
		auto m = apsParser.getDrumMixers()[i];
		auto drum = mpc.getDrum(i);

		for (int noteIndex = 0; noteIndex < 64; noteIndex++)
		{
			auto apssmc = m->getStereoMixerChannel(noteIndex);
			auto apsifmc = m->getIndivFxMixerChannel(noteIndex);
			auto drumsmc = drum->getStereoMixerChannels()[noteIndex].lock();
			auto drumifmc = drum->getIndivFxMixerChannels()[noteIndex].lock();
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

	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));

	mixerSetupScreen->setRecordMixChangesEnabled(apsParser.getGlobalParameters()->isRecordMixChangesEnabled());
	mixerSetupScreen->setCopyPgmMixToDrumEnabled(apsParser.getGlobalParameters()->isCopyPgmMixToDrumEnabled());
	mixerSetupScreen->setFxDrum(apsParser.getGlobalParameters()->getFxDrum());
	mixerSetupScreen->setIndivFxSourceDrum(apsParser.getGlobalParameters()->isIndivFxSourceDrum());
	mixerSetupScreen->setStereoMixSourceDrum(apsParser.getGlobalParameters()->isStereoMixSourceDrum());

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(mpc.screens->getScreenComponent("drum"));

	drumScreen->setPadToIntSound(apsParser.getGlobalParameters()->isPadToIntSoundEnabled());
	
	sampler->setSoundIndex(0);
	
	mpc.getLayeredScreen().lock()->openScreen("load");
	disk->setBusy(false);
}

void ApsLoader::loadSound(string soundFileName, string ext, mpc::disk::MpcFile* soundFile, bool replace, int loadSoundIndex)
{
	auto sl = mpc::disk::SoundLoader(mpc, mpc.getSampler().lock()->getSounds(), replace);
	sl.setPartOfProgram(true);
	showPopup(soundFileName, ext, soundFile->length());
	sl.loadSound(soundFile);
}

void ApsLoader::showPopup(string name, string ext, int sampleSize)
{
	mpc.getLayeredScreen().lock()->openScreen("popup");
	auto popupScreen = mpc.screens->get<PopupScreen>("popup");
	popupScreen->setText("LOADING " + StrUtil::toUpper(StrUtil::padRight(name, " ", 16) + "." + ext));

	if (dynamic_pointer_cast<mpc::disk::StdDisk>(mpc.getDisk().lock()))
	{
		auto sleepTime = sampleSize / 800;
	
		if (sleepTime < 300)
			sleepTime = 300;

		this_thread::sleep_for(chrono::milliseconds((int)(sleepTime * 0.2)));
	}
}

ApsLoader::~ApsLoader()
{
	if (loadThread.joinable())
		loadThread.join();
}
