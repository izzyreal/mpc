#include <disk/ProgramLoader.hpp>

#include <Mpc.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/PgmFileToProgramConverter.hpp>
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

using namespace mpc::sampler;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace moduru::lang;

std::shared_ptr<MpcFile> findSoundFileByFilenameWithoutExtension(mpc::Mpc& mpc, const std::string& filename, std::string& foundExtension)
{
    std::shared_ptr<MpcFile> result;

    auto disk = mpc.getDisk();

    for (auto &f: disk->getAllFiles()) {
        if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), filename + ".snd")) {
            result = f;
            foundExtension = "snd";
            break;
        }
    }

    if (!result || !result->exists()) {
        for (auto &f: disk->getAllFiles()) {
            if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), filename + ".wav")) {
                result = f;
                foundExtension = "wav";
                break;
            }
        }
    }

    return result;
}

program_or_error
ProgramLoader::loadProgram(mpc::Mpc &mpc, std::shared_ptr<mpc::disk::MpcFile> file, std::shared_ptr<mpc::sampler::Program> program)
{
    auto cantFindFileScreen = mpc.screens->get<CantFindFileScreen>("cant-find-file");
    cantFindFileScreen->skipAll = false;

    auto disk = mpc.getDisk();

    std::vector<std::string> pgmSoundNames;

    return PgmFileToProgramConverter::loadFromFileAndConvert(
            file,
            program,
            pgmSoundNames
    ).map([pgmSoundNames, disk, &mpc](std::shared_ptr<Program> p)
    {
        std::vector<std::pair<int, std::string>> localTable;

        for (int i = 0; i < pgmSoundNames.size(); i++)
        {
            auto soundFileName = StrUtil::trim(pgmSoundNames[i]);
            localTable.push_back({i, soundFileName});
        }

        std::vector<std::pair<int, std::string>> globalTable;

        for (int i = 0; i < mpc.getSampler()->getSoundCount(); i++)
        {
            globalTable.push_back({i, mpc.getSampler()->getSoundName(i)});
        }

        std::vector<int> unavailableLocalSoundIndices;

        for (auto& localEntry : localTable)
        {
            std::string foundExtension;
            auto soundFile = findSoundFileByFilenameWithoutExtension(mpc, localEntry.second, foundExtension);

            if (!soundFile || !soundFile->exists())
            {
                unavailableLocalSoundIndices.push_back(localEntry.first);
                notFound(mpc, localEntry.second);
                continue;
            }

            const auto loadAProgramScreen = mpc.screens->get<LoadAProgramScreen>("load-a-program");
            const bool replaceExistingSameNamedSounds = loadAProgramScreen->loadReplaceSameSound;

            SoundLoader soundLoader(mpc, replaceExistingSameNamedSounds);

            showLoadingSoundNamePopup(mpc, localEntry.second, foundExtension, soundFile->length());

            SoundLoaderResult soundLoaderResult;
            const bool shouldBeConverted = false;
            auto sound = mpc.getSampler()->addSound();
            soundLoader.loadSound(soundFile, soundLoaderResult, sound, shouldBeConverted);

            if (!soundLoaderResult.success)
            {
                mpc.getSampler()->deleteSoundWithoutRepairingPrograms(sound);
            }
        }

        std::vector<std::pair<int, std::string>> convertedTable = localTable;

        for (auto& convertedEntry : convertedTable)
        {
            const auto localSoundName = convertedEntry.second;
            bool wasFoundInGlobalTable = false;

            for (auto& globalEntry : globalTable)
            {
                // In all cases where the sampler already has a sound by some name,
                // we will want the loaded program to refer to this sound's index.
                if (localSoundName == globalEntry.second)
                {
                    convertedEntry.first = globalEntry.first;
                    wasFoundInGlobalTable = true;
                    break;
                }
            }

            if (!wasFoundInGlobalTable)
            {
                convertedEntry.first = -1;

                for (int sampleIndex = 0; sampleIndex < mpc.getSampler()->getSoundCount(); sampleIndex++)
                {
                    if (mpc.getSampler()->getSoundName(sampleIndex) == localSoundName)
                    {
                        convertedEntry.first = sampleIndex;
                        break;
                    }
                }
            }
        }

        for (auto &srcNoteParams: p->getNotesParameters())
        {
            auto localSoundIndex = srcNoteParams->getSoundIndex();

            std::string localSoundName;

            for (auto& localEntry : localTable)
            {
                if (localEntry.first == localSoundIndex)
                {
                    localSoundName = localEntry.second;
                    break;
                }
            }

            srcNoteParams->setSoundIndex(-1);

            if (!localSoundName.empty())
            {
                for (auto& convertedEntry : convertedTable)
                {
                    if (convertedEntry.second == localSoundName)
                    {
                        srcNoteParams->setSoundIndex(convertedEntry.first);
                        break;
                    }
                }
            }
        }

        mpc.getLayeredScreen()->openScreen("load");
        return p;
    });
}

void ProgramLoader::showLoadingSoundNamePopup(mpc::Mpc &mpc, std::string name, std::string ext, int sampleSize)
{
    mpc.getLayeredScreen()->openScreen("popup");
    auto popupScreen = mpc.screens->get<PopupScreen>("popup");
    popupScreen->setText("Loading " + StrUtil::padRight(name, " ", 16) + "." + StrUtil::toUpper(ext));

    auto sleepTime = sampleSize / 800;

    if (sleepTime < 300)
        sleepTime = 300;

    std::this_thread::sleep_for(std::chrono::milliseconds((int) (sleepTime * 0.2)));
}

void ProgramLoader::notFound(mpc::Mpc &mpc, std::string soundFileName)
{
    auto cantFindFileScreen = mpc.screens->get<CantFindFileScreen>("cant-find-file");
    auto skipAll = cantFindFileScreen->skipAll;

    if (!skipAll) {
        cantFindFileScreen->waitingForUser = true;

        cantFindFileScreen->fileName = soundFileName;

        mpc.getLayeredScreen()->openScreen("cant-find-file");

        while (cantFindFileScreen->waitingForUser)
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}
