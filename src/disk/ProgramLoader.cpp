#include <disk/ProgramLoader.hpp>

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/PgmToProgramConverter.hpp>
#include <disk/ProgramImportAdapter.hpp>
#include <disk/SoundLoader.hpp>
#include <ui/disk/DiskGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <lang/StrUtil.hpp>

using namespace moduru::lang;

using namespace mpc::disk;
using namespace std;

ProgramLoader::ProgramLoader(mpc::Mpc* mpc, MpcFile* file, bool replace)
{
	this->mpc = mpc;
	this->file = file;
	this->replace = replace;
	if (loadProgramThread.joinable()) loadProgramThread.join();
	loadProgramThread = thread(&ProgramLoader::static_loadProgram, this);
}

void ProgramLoader::static_loadProgram(void* this_p)
{
	static_cast<ProgramLoader*>(this_p)->loadProgram();
}

void ProgramLoader::loadProgram()
{
	auto converter = PgmToProgramConverter(file, mpc->getSampler());
	auto p = converter.get();
	auto pgmSoundNames = converter.getSoundNames();
	auto soundsDestIndex = vector<int>(pgmSoundNames.size());
	auto lDisk = mpc->getDisk().lock();
	for (int i = 0; i < pgmSoundNames.size(); i++) {
		auto ext = "snd";
		MpcFile* soundFile = nullptr;
		auto soundFileName = StrUtil::replaceAll(pgmSoundNames[i], ' ', "");
		for (auto& f : lDisk->getAllFiles()) {
			if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".SND"))
				soundFile = f;
		}
		if (soundFile == nullptr) {
			for (auto& f : lDisk->getAllFiles()) {
				if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".WAV"))
					soundFile = f;
			}
			ext = "wav";
		}
		if (soundFile != nullptr) {
			loadSound(soundFileName, ext, soundFile, &soundsDestIndex, mpc, replace, i);
		}
		else {
			if (lDisk != nullptr) {
				ext = "snd";
				soundFileName = soundFileName.substr(0, 8);
				for (auto& f : lDisk->getAllFiles()) {
					if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".SND"))
						soundFile = f;
				}
				if (soundFile == nullptr) {
					for (auto& f : lDisk->getAllFiles()) {
						if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".WAV"))
							soundFile = f;
					}
					ext = "wav";
				}
			}
			if (soundFile == nullptr)
				notfound(soundFileName, ext);
		}
	}

	auto adapter = ProgramImportAdapter(mpc->getSampler(), p, soundsDestIndex);
	result = adapter.get();
	mpc->importLoadedProgram();
	mpc->getUis().lock()->getDiskGui()->removePopup();
	lDisk->setBusy(false);
	mpc->getLayeredScreen().lock()->openScreen("load");
}

void ProgramLoader::loadSound(string soundFileName, string ext, MpcFile* soundFile, vector<int>* soundsDestIndex, mpc::Mpc* mpc, bool replace, int loadSoundIndex)
{
	int addedSoundIndex = -1;
	auto sl = SoundLoader(mpc, mpc->getSampler().lock()->getSounds(), replace);
	sl.setPartOfProgram(true);
	try {
		addedSoundIndex = sl.loadSound(soundFile);
		if (addedSoundIndex != -1) {
			showPopup(soundFileName, ext, soundFile->length());
			(*soundsDestIndex)[loadSoundIndex] = addedSoundIndex;
		}
	}
	catch (exception e) {
		e.what();
	}
}

void ProgramLoader::showPopup(string name, string ext, int sampleSize)
{
	mpc->getUis().lock()->getDiskGui()->openPopup(StrUtil::padRight(name, " ", 16), ext);
	if (dynamic_pointer_cast<StdDisk>(mpc->getDisk().lock()) != nullptr) {
		try {
			auto sleepTime = sampleSize / 400;
			if (sleepTime < 300) sleepTime = 300;
			//this_thread::sleep_for(chrono::milliseconds((int)(sleepTime * mpc::maingui::Constants::TFACTOR)));
			this_thread::sleep_for(chrono::milliseconds((int)(sleepTime * 1.0)));
		}
		catch (exception e) {
			e.what();
		}
	}
}

void ProgramLoader::notfound(string soundFileName, string ext)
{
	auto diskGui = mpc->getUis().lock()->getDiskGui();
	auto skipAll = diskGui->getSkipAll();
	if (!skipAll) {
		diskGui->openPopup(soundFileName, ext);
		try {
			this_thread::sleep_for(chrono::milliseconds(500));
		}
		catch (exception e) {
			e.what();
		}
		mpc->getLayeredScreen().lock()->removePopup();
		diskGui->setWaitingForUser(true);
		diskGui->setCannotFindFileName(soundFileName);
		mpc->getLayeredScreen().lock()->openScreen("cantfindfile");
		while (diskGui->isWaitingForUser()) {
			try {
				this_thread::sleep_for(chrono::milliseconds(25));
			}
			catch (exception e) {
				e.what();
			}
		}
	}
}

weak_ptr<mpc::sampler::Program> ProgramLoader::get()
{
    return result;
}

ProgramLoader::~ProgramLoader() {
	if (loadProgramThread.joinable()) loadProgramThread.join();
}