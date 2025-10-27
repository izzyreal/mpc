#include "AutoSave.hpp"

#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "mpc_fs.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/ApsLoader.hpp"
#include "file/aps/ApsParser.hpp"
#include "file/sndreader/SndReader.hpp"
#include "file/sndwriter/SndWriter.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"
#include "lcdgui/screens/window/VmpcContinuePreviousSessionScreen.hpp"
#include "lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp"
#include "lcdgui/screens/window/EditSoundScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/Screens.hpp"

#include "disk/AllLoader.hpp"
#include "file/all/AllParser.hpp"
#include "DirectorySaveTarget.hpp"

#include <StrUtil.hpp>
#include <memory>

using namespace mpc;
using namespace mpc::file::all;
using namespace mpc::file::aps;
using namespace mpc::file::sndwriter;
using namespace mpc::file::sndreader;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

void AutoSave::restoreAutoSavedStateWithTarget(Mpc& mpc, std::shared_ptr<SaveTarget> saveTarget)
{
    auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>();
    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 0) return;

    const std::vector<fs::path> files{
        "APS.APS", "ALL.ALL", "soundIndex.txt", "selectedPad.txt", "selectedNote.txt",
        "screen.txt", "focus.txt", "sounds.txt", "currentDir.txt"
    };

    std::vector<fs::path> availableFiles;
    for (const auto &f : files)
        if (saveTarget->exists(f))
            availableFiles.push_back(f);

    if (availableFiles.empty()) return;

    const auto restoreAction = [&mpc, saveTarget, availableFiles]
    {
        std::map<fs::path, std::vector<char>> processInOrder;

        for (const auto &f : availableFiles)
        {
            auto size = saveTarget->fileSize(f);
            if (size == 0) continue;
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
                auto soundNames = StrUtil::split(std::string(data.begin(), data.end()), '\n');
                for (auto &soundName : soundNames)
                {
                    if (soundName.empty()) continue;
                    auto soundData = saveTarget->getFileData(soundName);
                    SndReader sndReader(soundData);
                    auto sound = mpc.getSampler()->addSound(sndReader.getSampleRate());
                    if (!sound) break;
                    sound->setMono(sndReader.isMono());
                    sndReader.readData(sound->getMutableSampleData());
                    sound->setName(sndReader.getName());
                    sound->setTune(sndReader.getTune());
                    sound->setLevel(sndReader.getLevel());
                    sound->setStart(sndReader.getStart());
                    sound->setEnd(sndReader.getEnd());
                    sound->setLoopTo(sound->getEnd() - sndReader.getLoopLength());
                    sound->setBeatCount(sndReader.getNumberOfBeats());
                    sound->setLoopEnabled(sndReader.isLoopEnabled());
                }
            }
            else processInOrder[f] = data;
        }

        const auto setIntProperty = [&](const std::string &file, const std::function<void(int)> &setter)
        {
            if (processInOrder.count(file))
                setter(processInOrder[file][0]);
        };

        const auto getStringProperty = [&](const std::string &file)
        {
            if (!processInOrder.count(file)) return std::string{};
            return std::string(processInOrder[file].begin(), processInOrder[file].end());
        };

        setIntProperty("soundIndex.txt", [&](int v){ mpc.getSampler()->setSoundIndex(v); });
        setIntProperty("selectedNote.txt", [&](int v){ mpc.clientEventController->setSelectedNote(v); });
        setIntProperty("selectedPad.txt", [&](int v){ mpc.clientEventController->setSelectedPad(v); });

        auto currentDir = fs::path(getStringProperty("currentDir.txt"));
        auto relativePath = fs::relative(currentDir, mpc.paths->defaultLocalVolumePath());
        for (auto &seg : relativePath) {
            mpc.getDisk()->moveForward(seg.string());
            mpc.getDisk()->initFiles();
        }

        const auto screenName = getStringProperty("screen.txt");
        const auto focusName = getStringProperty("focus.txt");
        const auto layeredScreen = mpc.getLayeredScreen();

        layeredScreen->openScreen(screenName);
        if (!focusName.empty())
            layeredScreen->setFocus(focusName);

        for (auto &p : mpc.getSampler()->getPrograms()) {
            if (auto pl = p.lock()) {
                for (auto &n : pl->getNotesParameters())
                    if (n->getSoundIndex() >= mpc.getSampler()->getSoundCount())
                        n->setSoundIndex(-1);
            }
        }

        if (mpc.getSampler()->getProgramCount() == 0)
            mpc.getSampler()->addProgram(0);

        for (int i = 0; i < 4; i++) {
            auto d = mpc.getDrum(i);
            if (!mpc.getSampler()->getProgram(d.getProgram()))
                d.setProgram(0);
        }
    };

    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 1)
    {
        auto confirmScreen = mpc.screens->get<VmpcContinuePreviousSessionScreen>();
        confirmScreen->setRestoreAutoSavedStateAction(restoreAction);
        mpc.getLayeredScreen()->openScreen<VmpcContinuePreviousSessionScreen>();
        return;
    }

    restoreAction();
}

void AutoSave::storeAutoSavedStateWithTarget(Mpc& mpc, std::shared_ptr<SaveTarget> saveTarget)
{
    auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>();

    if (vmpcAutoSaveScreen->getAutoSaveOnExit() == 0 ||
        mpc.getLayeredScreen()->getCurrentScreenName() == "vmpc-continue-previous-session")
        return;

    const auto storeAction = [&]()
    {
        auto layeredScreen = mpc.getLayeredScreen();
        std::string currentScreen = layeredScreen->getFirstLayerScreenName();
        auto focus = layeredScreen->getFocusedFieldName();
        auto soundIndex = mpc.getSampler()->getSoundIndex();
        auto selectedPad = mpc.clientEventController->getSelectedPad();
        auto selectedNote = mpc.clientEventController->getSelectedNote();
        auto currentDir = mpc.getDisk()->getAbsolutePath();

        saveTarget->setFileData("screen.txt", {currentScreen.begin(), currentScreen.end()});
        saveTarget->setFileData("focus.txt", {focus.begin(), focus.end()});
        saveTarget->setFileData("currentDir.txt", {currentDir.begin(), currentDir.end()});
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
            SndWriter sndWriter(sound.get());
            const auto sndPath = sound->getName() + ".SND";
            saveTarget->setFileData(sndPath, { sndWriter.getSndFileArray().begin(), sndWriter.getSndFileArray().end() });
            soundNames += sndPath + "\n";
        }

        {
            AllParser allParser(mpc);
            saveTarget->setFileData("ALL.ALL", allParser.getBytes());
        }

        saveTarget->setFileData("sounds.txt", {soundNames.begin(), soundNames.end()});
    };

    storeAction();
}

