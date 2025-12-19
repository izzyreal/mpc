#include "AutoSave.hpp"

#include "Mpc.hpp"
#include "SaveTarget.hpp"

#include "controller/ClientEventController.hpp"

#include "mpc_fs.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/ApsLoader.hpp"
#include "file/aps/ApsParser.hpp"
#include "file/sndreader/SndReader.hpp"
#include "file/sndwriter/SndWriter.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"
#include "lcdgui/screens/window/VmpcContinuePreviousSessionScreen.hpp"
#include "lcdgui/Screens.hpp"

#include "disk/AllLoader.hpp"
#include "file/all/AllParser.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

#include "StrUtil.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "sequencer/Transport.hpp"

#include <chrono>
#include <memory>

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
    const auto vmpcAutoSaveScreen =
        mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();
    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 0 &&
        !mpc.isPluginModeEnabled())
    {
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
        return;
    }

    const auto restoreAction = [&mpc, availableFiles, saveTarget, headless, this]
    {
        auto layeredScreen = mpc.getLayeredScreen();
        std::map<fs::path, std::vector<char>> processInOrder;

        auto showMsg = [&](const std::string &msg)
        {
            if (headless)
            {
                return;
            }

            layeredScreen->postToUiThread(utils::Task(
                [layeredScreen, msg]
                {
                    layeredScreen->showPopup(msg);
                }));

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        };

        for (const auto &f : availableFiles)
        {
            if (shouldStopRestore.load(std::memory_order_relaxed))
            {
                break;
            }

            if (auto size = saveTarget->fileSize(f); size == 0)
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
                constexpr bool withoutSounds = true;
                // Regardless of whether the outer context is headless, we want
                // the ApsLoader to always operate in headless mode so it
                // doesn't spawn popups. We manage popups ourselves here in
                // AutoSave.
                constexpr bool apsLoadingHeadless = true;
                disk::ApsLoader::loadFromParsedAps(
                    apsParser, mpc, withoutSounds, apsLoadingHeadless);
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
                       [&](const int v)
                       {
                           mpc.getSampler()->setSoundIndex(v);
                       });
        setIntProperty("selectedNote.txt",
                       [&](const int v)
                       {
                           mpc.clientEventController->setSelectedNote(
                               DrumNoteNumber(v));
                       });
        setIntProperty("selectedPad.txt",
                       [&](const int v)
                       {
                           mpc.clientEventController->setSelectedPad(
                               ProgramPadIndex(v));
                       });

        auto currentDir = fs::path(getStringProperty("currentDir.txt"));
        auto relativePath =
            fs::relative(currentDir, mpc.paths->getDocuments()->defaultLocalVolumePath());
        for (auto &seg : relativePath)
        {
            mpc.getDisk()->moveForward(seg.string());
            mpc.getDisk()->initFiles();
        }

        const auto screenName = getStringProperty("screen.txt");
        const auto focusName = getStringProperty("focus.txt");

        utils::Task uiTask;
        uiTask.set(
            [layeredScreen, screenName, focusName,
             sequencer = mpc.getSequencer()]
            {
                layeredScreen->openScreen(screenName);

                if (screengroups::isSongScreen(
                        layeredScreen->getCurrentScreen()))
                {
                    sequencer->getTransport()->setPosition(0);
                }

                if (!focusName.empty())
                {
                    layeredScreen->setFocus(focusName);
                }
            });
        layeredScreen->postToUiThread(std::move(uiTask));

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

        for (size_t drumBusIndex = 0;
             drumBusIndex < Mpc2000XlSpecs::DRUM_BUS_COUNT; ++drumBusIndex)
        {
            if (auto d = mpc.getSequencer()->getDrumBus(
                    sequencer::drumBusIndexToDrumBusType(drumBusIndex));
                !mpc.getSampler()->getProgram(d->getProgramIndex())->isUsed())
            {
                d->setProgramIndex(ProgramIndex{0});
            }
        }
    };

    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 1 &&
        !mpc.isPluginModeEnabled())
    {
        const auto confirmScreen =
            mpc.screens->get<ScreenId::VmpcContinuePreviousSessionScreen>();
        confirmScreen->setRestoreAutoSavedStateAction(restoreAction);
        mpc.getLayeredScreen()->openScreenById(
            ScreenId::VmpcContinuePreviousSessionScreen);
        return;
    }

    if (mpc.isPluginModeEnabled())
    {
        restoreAction();
    }
    else
    {
        restoreThread = std::thread(
            [restoreAction]
            {
                restoreAction();
            });
    }
}

void AutoSave::storeAutoSavedState(
    Mpc &mpc, const std::shared_ptr<SaveTarget> &saveTarget)
{
    if (const auto vmpcAutoSaveScreen =
            mpc.screens->get<ScreenId::VmpcAutoSaveScreen>();
        vmpcAutoSaveScreen->getAutoSaveOnExit() == 0 ||
        mpc.getLayeredScreen()->getCurrentScreenName() ==
            "vmpc-continue-previous-session")
    {
        return;
    }

    const auto storeAction = [&mpc, saveTarget]
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
        saveTarget->setFileData("soundIndex.txt",
                                {static_cast<char>(soundIndex)});
        saveTarget->setFileData("selectedNote.txt",
                                {static_cast<char>(selectedNote)});
        saveTarget->setFileData("selectedPad.txt",
                                {static_cast<char>(selectedPad)});

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

void AutoSave::interruptRestorationIfStillOngoing()
{
    shouldStopRestore.store(false, std::memory_order_relaxed);
}

AutoSave::~AutoSave()
{
    if (restoreThread.joinable())
    {
        restoreThread.join();
    }
}
