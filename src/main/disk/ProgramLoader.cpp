#include <disk/ProgramLoader.hpp>

#include <Mpc.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/PgmToProgramConverter.hpp>
#include <disk/ProgramImportAdapter.hpp>
#include <disk/SoundLoader.hpp>
#include <sampler/Program.hpp>
#include <sequencer/Track.hpp>

#include <lcdgui/screens/window/LoadAProgramScreen.hpp>
#include <lcdgui/screens/window/CantFindFileScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lang/StrUtil.hpp>

#include <thread>

#ifdef __linux__
#include <pthread.h>
#endif // __linux__

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::lang;

std::shared_ptr<mpc::sampler::Program> ProgramLoader::loadProgram(mpc::Mpc& mpc, std::shared_ptr<mpc::disk::MpcFile> file, const int replaceIndex)
{
    const bool replace = replaceIndex != -1;
    
    auto cantFindFileScreen = mpc.screens->get<CantFindFileScreen>("cant-find-file");
    cantFindFileScreen->skipAll = false;
    
	auto disk = mpc.getDisk();

    std::shared_ptr<sampler::Program> p;
    
	try
	{
		PgmToProgramConverter converter(file, mpc.getSampler(), replaceIndex);
		p = converter.get().lock();
		auto pgmSoundNames = converter.getSoundNames();
		auto soundsDestIndex = std::vector<int>(pgmSoundNames.size());

        std::vector<int> unavailableSoundIndices;
        std::map<int, int> finalSoundIndices;
        
        int skipCount = 0;

		for (int i = 0; i < pgmSoundNames.size(); i++)
		{
			auto ext = "snd";
            std::shared_ptr<MpcFile> soundFile;
            std::string soundFileName = StrUtil::replaceAll(pgmSoundNames[i], ' ', "");

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
				notFound(mpc, soundFileName, ext);
				continue;
			}

            finalSoundIndices[i] = i - skipCount;
            
            loadSound(mpc,
                      soundFileName,
                      pgmSoundNames[i],
                      ext,
                      soundFile,
                      &soundsDestIndex,
                      replace,
                      i);
        }

		ProgramImportAdapter adapter(mpc.getSampler(), p, soundsDestIndex, unavailableSoundIndices);
		        
        for (auto& noteParameters : p->getNotesParameters()) {
            
            if (noteParameters->getSoundIndex() == -1) continue;
                        
            if (finalSoundIndices.find(noteParameters->getSoundIndex()) == end(finalSoundIndices)) continue;
            
            auto finalSoundIndex = finalSoundIndices[noteParameters->getSoundIndex()];

            noteParameters->setSoundIndex(finalSoundIndex);
        }

        auto track = mpc.getSequencer()->getActiveTrack();
        auto loadAProgramScreen = mpc.screens->get<LoadAProgramScreen>("load-a-program");

        if (!loadAProgramScreen->clearProgramWhenLoading)
            mpc.getDrum(track->getBus() - 1)->setProgram(mpc.getSampler()->getProgramCount() - 1);
        
		mpc.getLayeredScreen()->openScreen("load");
	}
	catch (const std::exception&)
	{
		auto popupScreen = mpc.screens->get<PopupScreen>("popup");
		popupScreen->setText("Wrong file format");
		popupScreen->returnToScreenAfterInteraction("load");
		mpc.getLayeredScreen()->openScreen("popup");
        throw;
	}
    
    return p;
}

void ProgramLoader::loadSound(mpc::Mpc& mpc, const std::string& soundFileName, const std::string& soundName,
 const std::string& ext, std::shared_ptr<MpcFile> soundFile, std::vector<int>* soundsDestIndex,
 const bool replace, const int loadSoundIndex)
{
    SoundLoader soundLoader(mpc, mpc.getSampler()->getSounds(), replace);
    soundLoader.setPartOfProgram(true);
    showPopup(mpc, soundName, ext, soundFile->length());
    SoundLoaderResult soundLoaderResult;
    bool shouldBeConverted = false;

    try
    {
        soundLoader.loadSound(soundFile, soundLoaderResult, shouldBeConverted);
        
        if (soundLoaderResult.existingIndex != -1)
            (*soundsDestIndex)[loadSoundIndex] = soundLoaderResult.existingIndex;
    }
    catch (const std::exception& e)
    {
        auto msg = std::string(e.what());
        MLOG("Exception occurred in ProgramLoader::loadSound(...) -- " + msg);
        MLOG(soundLoaderResult.errorMessage);
        throw;
    }
}

void ProgramLoader::showPopup(mpc::Mpc& mpc, std::string name, std::string ext, int sampleSize)
{
	mpc.getLayeredScreen()->openScreen("popup");
	auto popupScreen = mpc.screens->get<PopupScreen>("popup");
	popupScreen->setText("Loading " + StrUtil::padRight(name, " ", 16) + "." + StrUtil::toUpper(ext));

	auto sleepTime = sampleSize / 800;
	
	if (sleepTime < 300)
		sleepTime = 300;

    std::this_thread::sleep_for(std::chrono::milliseconds((int)(sleepTime * 0.2)));
}

void ProgramLoader::notFound(mpc::Mpc& mpc, std::string soundFileName, std::string ext)
{
	auto cantFindFileScreen = mpc.screens->get<CantFindFileScreen>("cant-find-file");
	auto skipAll = cantFindFileScreen->skipAll;

	if (!skipAll)
	{
		cantFindFileScreen->waitingForUser = true;
		
		cantFindFileScreen->fileName = soundFileName;
		
		mpc.getLayeredScreen()->openScreen("cant-find-file");

		while (cantFindFileScreen->waitingForUser)
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
}
