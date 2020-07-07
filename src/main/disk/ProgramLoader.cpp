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

ProgramLoader::ProgramLoader(MpcFile* file, bool replace)
{
	this->file = file;
	this->replace = replace;

	auto cantFindFileScreen = dynamic_pointer_cast<CantFindFileScreen>(Screens::getScreenComponent("cant-find-file"));
	cantFindFileScreen->skipAll = false;

	loadProgramThread = thread(&ProgramLoader::static_loadProgram, this);
}

void ProgramLoader::static_loadProgram(void* this_p)
{
	static_cast<ProgramLoader*>(this_p)->loadProgram();
}

void ProgramLoader::loadProgram()
{
	auto converter = PgmToProgramConverter(file, Mpc::instance().getSampler());
	auto p = converter.get();
	auto pgmSoundNames = converter.getSoundNames();
	auto soundsDestIndex = vector<int>(pgmSoundNames.size());
	auto disk = Mpc::instance().getDisk().lock();
	
	for (int i = 0; i < pgmSoundNames.size(); i++)
	{
		auto ext = "snd";
		MpcFile* soundFile = nullptr;
		auto soundFileName = StrUtil::replaceAll(pgmSoundNames[i], ' ', "");
		
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
		
		if (soundFile != nullptr)
		{
			loadSound(soundFileName, ext, soundFile, &soundsDestIndex, replace, i);
		}
		else
		{
			if (disk != nullptr)
			{
				ext = "snd";
				soundFileName = soundFileName.substr(0, 8);
				
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
			}
			
			if (soundFile == nullptr)
			{
				notfound(soundFileName, ext);
			}
		}
	}

	auto adapter = ProgramImportAdapter(Mpc::instance().getSampler(), p, soundsDestIndex);
	result = adapter.get();
	Mpc::instance().importLoadedProgram();
	disk->setBusy(false);
	Mpc::instance().getLayeredScreen().lock()->openScreen("load");
}

void ProgramLoader::loadSound(string soundFileName, string ext, MpcFile* soundFile, vector<int>* soundsDestIndex, bool replace, int loadSoundIndex)
{
	int addedSoundIndex = -1;
	auto sl = SoundLoader(Mpc::instance().getSampler().lock()->getSounds(), replace);
	sl.setPartOfProgram(true);

	try
	{
		showPopup(soundFileName, ext, soundFile->length());
		addedSoundIndex = sl.loadSound(soundFile);
		
		if (addedSoundIndex != -1)
		{
			(*soundsDestIndex)[loadSoundIndex] = addedSoundIndex;
		}
	}
	catch (const exception& e)
	{
		auto msg = string(e.what());
		MLOG("Exception occurred in ProgramLoader::loadSound(...) -- " + msg);
	}
}

void ProgramLoader::showPopup(string name, string ext, int sampleSize)
{
	Mpc::instance().getLayeredScreen().lock()->openScreen("popup");
	auto popupScreen = dynamic_pointer_cast<PopupScreen>(Screens::getScreenComponent("popup"));
	popupScreen->setText("LOADING " + StrUtil::toUpper(StrUtil::padRight(name, " ", 16) + "." + ext));

	auto sleepTime = sampleSize / 800;
	
	if (sleepTime < 300)
	{
		sleepTime = 300;
	}

	this_thread::sleep_for(chrono::milliseconds((int)(sleepTime * 0.2)));
}

void ProgramLoader::notfound(string soundFileName, string ext)
{
	auto cantFindFileScreen = dynamic_pointer_cast<CantFindFileScreen>(Screens::getScreenComponent("cant-find-file"));
	auto skipAll = cantFindFileScreen->skipAll;

	if (!skipAll)
	{
		cantFindFileScreen->waitingForUser = true;
		
		cantFindFileScreen->fileName = soundFileName;
		
		Mpc::instance().getLayeredScreen().lock()->openScreen("cant-find-file");

		while (cantFindFileScreen->waitingForUser)
		{
			this_thread::sleep_for(chrono::milliseconds(25));
		}
	}
}

weak_ptr<mpc::sampler::Program> ProgramLoader::get()
{
    return result;
}

ProgramLoader::~ProgramLoader()
{
	if (loadProgramThread.joinable())
	{
		loadProgramThread.join();
	}
}