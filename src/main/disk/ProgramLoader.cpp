#include <disk/ProgramLoader.hpp>

#include <Mpc.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/PgmToProgramConverter.hpp>
#include <disk/ProgramImportAdapter.hpp>
#include <disk/SoundLoader.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <lcdgui/screens/window/CantFindFileScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <lang/StrUtil.hpp>

#ifdef __linux__
#include <pthread.h>
#endif // __linux__

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::lang;
using namespace std;

// Only used by Mpc
ProgramLoader::ProgramLoader(mpc::Mpc& mpc, MpcFile* file, const int replaceIndex)
	: mpc(mpc)
{
	this->file = file;
	this->replace = replace;

	auto cantFindFileScreen = dynamic_pointer_cast<CantFindFileScreen>(mpc.screens->getScreenComponent("cant-find-file"));
	cantFindFileScreen->skipAll = false;

	loadProgramThread = thread(&ProgramLoader::static_loadProgram, this, replaceIndex);
}

void ProgramLoader::static_loadProgram(void* this_p, const int replaceIndex)
{
	static_cast<ProgramLoader*>(this_p)->loadProgram(replaceIndex);
}

void ProgramLoader::loadProgram(const int replaceIndex)
{
	auto disk = mpc.getDisk().lock();

	try
	{
		PgmToProgramConverter converter(file, mpc.getSampler(), replaceIndex);
		auto p = converter.get();
		auto pgmSoundNames = converter.getSoundNames();
		auto soundsDestIndex = vector<int>(pgmSoundNames.size());

		vector<int> unavailableSoundIndices;

		for (int i = 0; i < pgmSoundNames.size(); i++)
		{
			auto ext = "snd";
			mpc::disk::MpcFile* soundFile = nullptr;
			string soundFileName = StrUtil::replaceAll(pgmSoundNames[i], ' ', "");

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

			loadSound(soundFileName, pgmSoundNames[i], ext, soundFile, &soundsDestIndex, replace, i);
		}

		auto adapter = ProgramImportAdapter(mpc.getSampler(), p, soundsDestIndex, unavailableSoundIndices);
		result = adapter.get();
		mpc.importLoadedProgram();
		disk->setBusy(false);
		mpc.getLayeredScreen().lock()->openScreen("load");
	}
	catch (const exception& e)
	{
		disk->setBusy(false);
		auto popupScreen = mpc.screens->get<PopupScreen>("popup");
		popupScreen->setText("Wrong file format");
		popupScreen->returnToScreenAfterInteraction("load");
		mpc.getLayeredScreen().lock()->openScreen("popup");
	}
}

void ProgramLoader::loadSound(const string& soundFileName, const string& soundName, const string& ext, MpcFile* soundFile, vector<int>* soundsDestIndex, const bool replace, const int loadSoundIndex)
{
	int addedSoundIndex = -1;
	SoundLoader sl(mpc, mpc.getSampler().lock()->getSounds(), replace);
	sl.setPartOfProgram(true);

	try
	{
		showPopup(soundName, ext, soundFile->length());
		addedSoundIndex = sl.loadSound(soundFile);
		
		if (addedSoundIndex != -1)
			(*soundsDestIndex)[loadSoundIndex] = addedSoundIndex;
	}
	catch (const exception& e)
	{
		auto msg = string(e.what());
		MLOG("Exception occurred in ProgramLoader::loadSound(...) -- " + msg);
	}
}

void ProgramLoader::showPopup(string name, string ext, int sampleSize)
{
	mpc.getLayeredScreen().lock()->openScreen("popup");
	auto popupScreen = mpc.screens->get<PopupScreen>("popup");
	popupScreen->setText("Loading " + StrUtil::padRight(name, " ", 16) + "." + StrUtil::toUpper(ext));

	auto sleepTime = sampleSize / 800;
	
	if (sleepTime < 300)
		sleepTime = 300;

	this_thread::sleep_for(chrono::milliseconds((int)(sleepTime * 0.2)));
}

void ProgramLoader::notFound(string soundFileName, string ext)
{
	auto cantFindFileScreen = dynamic_pointer_cast<CantFindFileScreen>(mpc.screens->getScreenComponent("cant-find-file"));
	auto skipAll = cantFindFileScreen->skipAll;

	if (!skipAll)
	{
		cantFindFileScreen->waitingForUser = true;
		
		cantFindFileScreen->fileName = soundFileName;
		
		mpc.getLayeredScreen().lock()->openScreen("cant-find-file");

		while (cantFindFileScreen->waitingForUser)
			this_thread::sleep_for(chrono::milliseconds(25));
	}
}

weak_ptr<mpc::sampler::Program> ProgramLoader::get()
{
    return result;
}

ProgramLoader::~ProgramLoader()
{
	if (loadProgramThread.joinable())
		loadProgramThread.join();
}