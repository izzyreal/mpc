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
ProgramLoader::ProgramLoader(mpc::Mpc& _mpc, weak_ptr<MpcFile> _file, const int replaceIndex)
: mpc(_mpc), file(_file), replace (replaceIndex != -1)
{
    auto cantFindFileScreen = mpc.screens->get<CantFindFileScreen>("cant-find-file");
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
        map<int, int> finalSoundIndices;
        
        int skipCount = 0;

		for (int i = 0; i < pgmSoundNames.size(); i++)
		{
			auto ext = "snd";
			shared_ptr<MpcFile> soundFile;
			string soundFileName = StrUtil::replaceAll(pgmSoundNames[i], ' ', "");

			for (auto& f : disk->getAllFiles())
			{
				if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".SND"))
				{
					soundFile = f;
					break;
				}
			}

			if (!soundFile || !soundFile->exists())
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

			if (!soundFile || !soundFile->exists())
			{
				unavailableSoundIndices.push_back(i);
                skipCount++;
				notFound(soundFileName, ext);
				continue;
			}

            finalSoundIndices[i] = i - skipCount;
            
            loadSound(
                      soundFileName,
                      pgmSoundNames[i],
                      ext,
                      soundFile,
                      &soundsDestIndex,
                      replace,
                      i
                      );
        }

		auto adapter = ProgramImportAdapter(mpc.getSampler(), p, soundsDestIndex, unavailableSoundIndices);
		
        result = adapter.get();
        
        for (auto& noteParameters : result.lock()->getNotesParameters()) {
            
            if (noteParameters->getSoundIndex() == -1) continue;
                        
            if (finalSoundIndices.find(noteParameters->getSoundIndex()) == end(finalSoundIndices)) continue;
            
            auto finalSoundIndex = finalSoundIndices[noteParameters->getSoundIndex()];

            noteParameters->setSoundIndex(finalSoundIndex);
        }

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

void ProgramLoader::loadSound
(
 const string& soundFileName,
 const string& soundName,
 const string& ext,
 weak_ptr<MpcFile> soundFile,
 vector<int>* soundsDestIndex,
 const bool replace,
 const int loadSoundIndex
 )
{
    SoundLoader soundLoader(mpc, mpc.getSampler().lock()->getSounds(), replace);
    soundLoader.setPartOfProgram(true);
    auto file = soundFile.lock();
    showPopup(soundName, ext, file->length());
    SoundLoaderResult result;

    try
    {
        soundLoader.loadSound(file, result);
        
        if (result.existingIndex != -1)
            (*soundsDestIndex)[loadSoundIndex] = result.existingIndex;
    }
    catch (const exception& e)
    {
        auto msg = string(e.what());
        MLOG("Exception occurred in ProgramLoader::loadSound(...) -- " + msg);
        MLOG(result.errorMessage);
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
	auto cantFindFileScreen = mpc.screens->get<CantFindFileScreen>("cant-find-file");
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
