#include "AutoSave.hpp"

#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "controller/ClientEventController.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "mpc_fs.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/ApsLoader.hpp"
#include "file/aps/ApsParser.hpp"
#include "file/sndreader/SndReader.hpp"
#include "file/sndwriter/SndWriter.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"
#include "lcdgui/screens/window/VmpcContinuePreviousSessionScreen.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"

#include "disk/AllLoader.hpp"
#include "file/all/AllParser.hpp"
#include "DirectorySaveTarget.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

#include "StrUtil.hpp"

#include <chrono>
#include <memory>
#include <thread>

using namespace mpc;
using namespace mpc::file::all;
using namespace mpc::file::aps;
using namespace mpc::file::sndwriter;
using namespace mpc::file::sndreader;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

void AutoSave::restoreAutoSavedState(Mpc &mpc,
                                     std::shared_ptr<SaveTarget> saveTarget,
                                     const bool headless)
{
    auto onCompletionTask = [&]
    {
        mpc.getLayeredScreen()->postToUiThread(
            [&]
            {
                mpc.startMidiDeviceDetector();
                mpc.getEngineHost()->getAudioServer()->start();
            });
    };

    auto vmpcAutoSaveScreen = mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();
    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 0 &&
        !mpc.isPluginModeEnabled())
    {
        onCompletionTask();
        return;
    }

    const std::vector<fs::path> files{
        "APS.APS",         "ALL.ALL",          "soundIndex.txt",
        "selectedPad.txt", "selectedNote.txt", "screen.txt",
        "focus.txt",       "sounds.txt",       "currentDir.txt"};

    std::vector<fs::path> availableFiles;
    for (const auto &f : files)
    {
        if (saveTarget->exists(f))
        {
            availableFiles.push_back(f);
        }
    }

    if (availableFiles.empty())
    {
        onCompletionTask();
        return;
    }

    const auto restoreAction =
        [&mpc, availableFiles, saveTarget, headless, onCompletionTask]
    {
        auto layeredScreen = mpc.getLayeredScreen();
        std::map<fs::path, std::vector<char>> processInOrder;

        auto showMsg = [&](const std::string &msg)
        {
            if (headless)
            {
                return;
            }

            layeredScreen->postToUiThread(
                [layeredScreen, msg]
                {
                    layeredScreen->showPopup(msg);
                });

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        };

        for (const auto &f : availableFiles)
        {
            auto size = saveTarget->fileSize(f);
            if (size == 0)
            {
                continue;
            }

            std::string desc;
            if (f == "APS.APS")
            {
                desc = "programs and settings";
            }
            else if (f == "ALL.ALL")
            {
                desc = "sequence data";
            }
            else if (f == "sounds.txt")
            {
                desc = "sounds";
            }
            else if (f == "soundIndex.txt")
            {
                desc = "sound index";
            }
            else if (f == "selectedPad.txt")
            {
                desc = "selected pad";
            }
            else if (f == "selectedNote.txt")
            {
                desc = "selected note";
            }
            else if (f == "screen.txt")
            {
                desc = "screen state";
            }
            else if (f == "focus.txt")
            {
                desc = "focus field";
            }
            else if (f == "currentDir.txt")
            {
                desc = "current directory";
            }

            showMsg("Loading " + desc);

            auto data = saveTarget->getFileData(f);

            if (f == "APS.APS")
            {
                ApsParser apsParser(data);
                disk::ApsLoader::loadFromParsedAps(apsParser, mpc, true, true);
            }
            else if (f == "ALL.ALL")
            {
                AllParser allParser(mpc, data);
                disk::AllLoader::loadEverythingFromAllParser(mpc, allParser);
            }
            else if (f == "sounds.txt")
            {
                auto soundNames =
                    StrUtil::split(std::string(data.begin(), data.end()), '\n');
                for (auto &soundName : soundNames)
                {
                    if (soundName.empty())
                    {
                        continue;
                    }

                    showMsg("Loading " + soundName.substr(0, 20));

                    auto soundData = saveTarget->getFileData(soundName);
                    SndReader sndReader(soundData);
                    auto sound =
                        mpc.getSampler()->addSound(sndReader.getSampleRate());
                    if (!sound)
                    {
                        break;
                    }

                    sound->setMono(sndReader.isMono());
                    sndReader.readData(sound->getMutableSampleData());
                    sound->setName(sndReader.getName());
                    sound->setTune(sndReader.getTune());
                    sound->setLevel(sndReader.getLevel());
                    sound->setStart(sndReader.getStart());
                    sound->setEnd(sndReader.getEnd());
                    sound->setLoopTo(sound->getEnd() -
                                     sndReader.getLoopLength());
                    sound->setBeatCount(sndReader.getNumberOfBeats());
                    sound->setLoopEnabled(sndReader.isLoopEnabled());
                }
            }
            else
            {
                processInOrder[f] = data;
            }
        }

        const auto setIntProperty =
            [&](const std::string &file, const std::function<void(int)> &setter)
        {
            if (processInOrder.count(file))
            {
                setter(processInOrder[file][0]);
            }
        };

        const auto getStringProperty = [&](const std::string &file)
        {
            if (!processInOrder.count(file))
            {
                return std::string{};
            }
            return std::string(processInOrder[file].begin(),
                               processInOrder[file].end());
        };

        setIntProperty("soundIndex.txt",
                       [&](int v)
                       {
                           mpc.getSampler()->setSoundIndex(v);
                       });
        setIntProperty("selectedNote.txt",
                       [&](int v)
                       {
                           mpc.clientEventController->setSelectedNote(v);
                       });
        setIntProperty("selectedPad.txt",
                       [&](int v)
                       {
                           mpc.clientEventController->setSelectedPad(v);
                       });

        auto currentDir = fs::path(getStringProperty("currentDir.txt"));
        auto relativePath =
            fs::relative(currentDir, mpc.paths->defaultLocalVolumePath());
        for (auto &seg : relativePath)
        {
            mpc.getDisk()->moveForward(seg.string());
            mpc.getDisk()->initFiles();
        }

        const auto screenName = getStringProperty("screen.txt");
        const auto focusName = getStringProperty("focus.txt");

        layeredScreen->postToUiThread(
            [layeredScreen, screenName, focusName]
            {
                layeredScreen->openScreen(screenName);

                if (!focusName.empty())
                {
                    layeredScreen->setFocus(focusName);
                }
            });

        for (auto &p : mpc.getSampler()->getPrograms())
        {
            if (auto pl = p.lock())
            {
                for (auto &n : pl->getNotesParameters())
                {
                    if (n->getSoundIndex() >= mpc.getSampler()->getSoundCount())
                    {
                        n->setSoundIndex(-1);
                    }
                }
            }
        }

        if (mpc.getSampler()->getProgramCount() == 0)
        {
            mpc.getSampler()->addProgram(0);
        }

        for (int i = 0; i < Mpc2000XlSpecs::DRUM_BUS_COUNT; i++)
        {
            auto d = mpc.getSequencer()->getDrumBus(i);
            if (!mpc.getSampler()->getProgram(d->getProgram()))
            {
                d->setProgram(0);
            }
        }

        onCompletionTask();
    };

    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 1 &&
        !mpc.isPluginModeEnabled())
    {
        auto confirmScreen =
            mpc.screens->get<ScreenId::VmpcContinuePreviousSessionScreen>();
        confirmScreen->setRestoreAutoSavedStateAction(restoreAction);
        mpc.getLayeredScreen()->openScreenById(
            ScreenId::VmpcContinuePreviousSessionScreen);
        return;
    }

    std::thread(
        [restoreAction]()
        {
            restoreAction();
        })
        .detach();
}

void AutoSave::storeAutoSavedState(Mpc &mpc,
                                   const std::shared_ptr<SaveTarget> saveTarget)
{
    auto vmpcAutoSaveScreen = mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();

    if (vmpcAutoSaveScreen->getAutoSaveOnExit() == 0 ||
        mpc.getLayeredScreen()->getCurrentScreenName() ==
            "vmpc-continue-previous-session")
    {
        return;
    }

    const auto storeAction = [&mpc, saveTarget]()
    {
        auto layeredScreen = mpc.getLayeredScreen();

        std::string currentScreen = layeredScreen->getFirstLayerScreenName();

        auto focus = layeredScreen->getFocusedFieldName();
        auto soundIndex = mpc.getSampler()->getSoundIndex();
        auto selectedPad = mpc.clientEventController->getSelectedPad();
        auto selectedNote = mpc.clientEventController->getSelectedNote();
        auto currentDir = mpc.getDisk()->getAbsolutePath();

        saveTarget->setFileData("screen.txt",
                                {currentScreen.begin(), currentScreen.end()});
        saveTarget->setFileData("focus.txt", {focus.begin(), focus.end()});
        saveTarget->setFileData("currentDir.txt",
                                {currentDir.begin(), currentDir.end()});
        saveTarget->setFileData("soundIndex.txt", {(char)soundIndex});
        saveTarget->setFileData("selectedNote.txt", {(char)selectedNote});
        saveTarget->setFileData("selectedPad.txt", {(char)selectedPad});

        {
            ApsParser apsParser(mpc, "stateinfo");
            saveTarget->setFileData("APS.APS", apsParser.getBytes());
        }

        std::string soundNames;
        for (auto &sound : mpc.getSampler()->getSounds())
        {
            const auto sndPath = sound->getName() + ".SND";
            SndWriter sndWriter(sound.get());
            saveTarget->setFileData(sndPath,
                                    {sndWriter.getSndFileArray().begin(),
                                     sndWriter.getSndFileArray().end()});
            soundNames += sndPath + "\n";
        }

        {
            AllParser allParser(mpc);
            saveTarget->setFileData("ALL.ALL", allParser.getBytes());
        }

        saveTarget->setFileData("sounds.txt",
                                {soundNames.begin(), soundNames.end()});
        layeredScreen->closeCurrentScreen();
    };

    storeAction();
}
