#include "disk/ProgramLoader.hpp"

#include "Mpc.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/PgmFileToProgramConverter.hpp"
#include "disk/SoundLoader.hpp"
#include "sampler/Program.hpp"

#include "lcdgui/screens/window/LoadAProgramScreen.hpp"
#include "lcdgui/screens/window/CantFindFileScreen.hpp"
#include "sampler/Sampler.hpp"

#include "StrUtil.hpp"

#include <thread>

#include "performance/PerformanceManager.hpp"

#ifdef __linux__
#include <pthread.hpp>
#endif // __linux__

using namespace mpc::sampler;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

std::shared_ptr<MpcFile>
findSoundFileByFilenameWithoutExtension(const mpc::Mpc &mpc,
                                        const std::string &filename,
                                        std::string &foundExtension)
{
    std::shared_ptr<MpcFile> result;

    const auto disk = mpc.getDisk();

    for (const auto &f : disk->getAllFiles())
    {
        if (mpc::StrUtil::eqIgnoreCase(
                mpc::StrUtil::replaceAll(f->getName(), ' ', ""),
                filename + ".snd"))
        {
            result = f;
            foundExtension = "snd";
            break;
        }
    }

    if (!result || !result->exists())
    {
        for (const auto &f : disk->getAllFiles())
        {
            if (mpc::StrUtil::eqIgnoreCase(
                    mpc::StrUtil::replaceAll(f->getName(), ' ', ""),
                    filename + ".wav"))
            {
                result = f;
                foundExtension = "wav";
                break;
            }
        }
    }

    return result;
}

program_or_error
ProgramLoader::loadProgram(Mpc &mpc, const std::shared_ptr<MpcFile> &file,
                           const std::shared_ptr<Program> &program)
{
    const auto cantFindFileScreen =
        mpc.screens->get<ScreenId::CantFindFileScreen>();
    cantFindFileScreen->skipAll = false;

    auto disk = mpc.getDisk();

    std::vector<std::string> pgmSoundNames;

    return PgmFileToProgramConverter::loadFromFileAndConvert(file, program,
                                                             pgmSoundNames)
        .map(
            [pgmSoundNames, &mpc](std::shared_ptr<Program> p)
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
                    globalTable.push_back(
                        {i, mpc.getSampler()->getSoundName(i)});
                }

                std::vector<int> unavailableLocalSoundIndices;

                for (auto &localEntry : localTable)
                {
                    std::string foundExtension;
                    auto soundFile = findSoundFileByFilenameWithoutExtension(
                        mpc, localEntry.second, foundExtension);

                    if (!soundFile || !soundFile->exists())
                    {
                        unavailableLocalSoundIndices.push_back(
                            localEntry.first);
                        notFound(mpc, localEntry.second);
                        continue;
                    }

                    const auto loadAProgramScreen =
                        mpc.screens->get<ScreenId::LoadAProgramScreen>();
                    const bool replaceExistingSameNamedSounds =
                        loadAProgramScreen->loadReplaceSameSound;

                    SoundLoader soundLoader(mpc,
                                            replaceExistingSameNamedSounds);

                    showLoadingSoundNamePopup(mpc, localEntry.second,
                                              foundExtension);

                    SoundLoaderResult soundLoaderResult;
                    constexpr bool shouldBeConverted = false;
                    auto sound = mpc.getSampler()->addSound();
                    soundLoader.loadSound(soundFile, soundLoaderResult, sound,
                                          shouldBeConverted);

                    if (!soundLoaderResult.success)
                    {
                        mpc.getSampler()->deleteSoundWithoutRepairingPrograms(
                            sound);
                    }
                }

                std::vector<std::pair<int, std::string>> convertedTable =
                    localTable;

                for (auto &convertedEntry : convertedTable)
                {
                    const auto localSoundName = convertedEntry.second;
                    bool wasFoundInGlobalTable = false;

                    for (auto &globalEntry : globalTable)
                    {
                        // In all cases where the sampler already has a sound by
                        // some name, we will want the loaded program to refer
                        // to this sound's index.
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

                        for (int sampleIndex = 0;
                             sampleIndex < mpc.getSampler()->getSoundCount();
                             sampleIndex++)
                        {
                            if (mpc.getSampler()->getSoundName(sampleIndex) ==
                                localSoundName)
                            {
                                convertedEntry.first = sampleIndex;
                                break;
                            }
                        }
                    }
                }

                for (auto &srcNoteParams : p->getNotesParameters())
                {
                    performance::AddProgramSound payload{srcNoteParams, localTable, convertedTable};
                    mpc.performanceManager->enqueue(performance::PerformanceMessage{payload});
                }

                auto ls = mpc.getLayeredScreen();
                ls->postToUiThread(
                    [ls]
                    {
                        ls->openScreenById(ScreenId::LoadScreen);
                    });
                return p;
            });
}

void ProgramLoader::showLoadingSoundNamePopup(Mpc &mpc, const std::string &name,
                                              const std::string &ext)
{
    const std::string msg =
        "Loading " +
        StrUtil::toUpper(StrUtil::padRight(name, " ", 16) + "." + ext);
    auto ls = mpc.getLayeredScreen();
    ls->postToUiThread(
        [ls, msg]
        {
            ls->showPopup(msg);
        });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void ProgramLoader::notFound(Mpc &mpc, const std::string &soundFileName)
{
    const auto cantFindFileScreen =
        mpc.screens->get<ScreenId::CantFindFileScreen>();

    if (const auto skipAll = cantFindFileScreen->skipAll; !skipAll)
    {
        cantFindFileScreen->waitingForUser = true;

        cantFindFileScreen->fileName = soundFileName;

        mpc.getLayeredScreen()->openScreenById(ScreenId::CantFindFileScreen);

        while (cantFindFileScreen->waitingForUser)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
        }
    }
}
