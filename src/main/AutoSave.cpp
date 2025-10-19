#include "AutoSave.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "mpc_fs.hpp"

#include <disk/AbstractDisk.hpp>
#include <disk/ApsLoader.hpp>
#include <file/aps/ApsParser.hpp>
#include <file/sndreader/SndReader.hpp>
#include <file/sndwriter/SndWriter.hpp>
#include <lcdgui/screens/VmpcAutoSaveScreen.hpp>
#include <lcdgui/screens/window/VmpcContinuePreviousSessionScreen.hpp>
#include <lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/Screens.hpp"

#include "disk/AllLoader.hpp"
#include "file/all/AllParser.hpp"

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

void AutoSave::restoreAutoSavedState(mpc::Mpc &mpc)
{
    auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>();

    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 0)
    {
        return;
    }

    const auto path = mpc.paths->autoSavePath();
    const auto apsFile = path / "APS.APS";
    const auto allFile = path / "ALL.ALL";
    const auto soundIndexFile = path / "soundIndex.txt";
    const auto lastPressedPadFile = path / "lastPressedPad.txt";
    const auto lastPressedNoteFile = path / "lastPressedNote.txt";
    const auto screenFile = path / "screen.txt";
    const auto previousScreenFile = path / "previousScreen.txt";
    const auto previousSamplerScreenFile = path / "previousSamplerScreen.txt";
    const auto focusFile = path / "focus.txt";
    const auto soundsFile = path / "sounds.txt";
    const auto currentDirFile = path / "currentDir.txt";

    std::vector<fs::path> files{apsFile, allFile, soundIndexFile, lastPressedPadFile, lastPressedNoteFile, screenFile, previousScreenFile, previousSamplerScreenFile, focusFile, soundsFile, currentDirFile };

    std::vector<fs::path> availableFiles;

    for (auto &f: files)
    {
        if (fs::exists(f))
        {
            availableFiles.push_back(f);
        }
    }

    if (availableFiles.empty())
    {
        return;
    }

    const auto restoreAutoSavedStateAction = [&mpc, availableFiles, path] {

        std::map<fs::path, std::vector<char>> processInOrder;

        for (auto &f : availableFiles)
        {
            auto size1 = fs::file_size(f);

            if (size1 == 0)
            {
                continue;
            }

            const auto data = get_file_data(f);

            if (f == path / "APS.APS")
            {
                ApsParser apsParser(data);
                disk::ApsLoader::loadFromParsedAps(apsParser, mpc, true, true);
            }
            else if (f == path / "ALL.ALL")
            {
                AllParser allParser(mpc, data);
                disk::AllLoader::loadEverythingFromAllParser(mpc, allParser);
            }
            else if (f == path / "sounds.txt")
            {
                auto soundNames = StrUtil::split(std::string(data.begin(), data.end()), '\n');

                for (auto &soundName: soundNames)
                {
                    const auto soundPath = mpc.paths->autoSavePath() / soundName;

                    auto soundData = get_file_data(soundPath);

                    SndReader sndReader(soundData);

                    auto sound = mpc.getSampler()->addSound(sndReader.getSampleRate(), "");

                    if (sound == nullptr)
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
                    sound->setLoopTo(sound->getEnd() - sndReader.getLoopLength());
                    sound->setBeatCount(sndReader.getNumberOfBeats());
                    sound->setLoopEnabled(sndReader.isLoopEnabled());
                }
            }
            else
            {
                processInOrder[f] = data;
            }
        }

        const auto setIntProperty = [&processInOrder, path](const std::string& prop, const std::function<void(int v)>& setter) {
            if (processInOrder.find(path / prop) != processInOrder.end())
            {
                std::vector<char> str = processInOrder[path / prop];
                try
                {
                    auto value = str[0];
                    setter(value);
                } catch (const std::exception &)
                {}
            }
        };

        const auto getStringProperty = [&processInOrder, path](const std::string& prop) -> std::string {
            if (processInOrder.find(path / prop) != processInOrder.end())
            {
                return {processInOrder[path / prop].begin(), processInOrder[path / prop].end()};
            } else return {};
        };

        setIntProperty("soundIndex.txt", [&mpc](int v){mpc.getSampler()->setSoundIndex(v);});
        setIntProperty("lastPressedNote.txt", [&mpc](int v){mpc.setNote(v);});
        setIntProperty("lastPressedPad.txt", [&mpc](int v){mpc.setPad(v);});

        auto currentDir = fs::path(getStringProperty("currentDir.txt"));

        auto relativePath = fs::relative(currentDir, mpc.paths->defaultLocalVolumePath());

        for (auto& pathSegment : relativePath)
        {
            mpc.getDisk()->moveForward(pathSegment.string());
            mpc.getDisk()->initFiles();
        }

        const auto screenName = getStringProperty("screen.txt");
        const auto previousScreenName = getStringProperty("previousScreen.txt");
        const auto previousSamplerScreenName = getStringProperty("previousSamplerScreen.txt");

        const auto focusName = getStringProperty("focus.txt");

        const auto layeredScreen = mpc.getLayeredScreen();
        const auto currentScreenName = layeredScreen->getCurrentScreenName();
        const auto currentScreen = mpc.screens->getByName1("currentScreenName");

        layeredScreen->openScreen(previousSamplerScreenName);
        layeredScreen->Draw();
        layeredScreen->openScreen(previousScreenName);
        layeredScreen->Draw();

        layeredScreen->openScreen(screenName);
        layeredScreen->Draw();

        if (!focusName.empty())
        {
            layeredScreen->setFocus(focusName);
        }

        if (std::dynamic_pointer_cast<VmpcKnownControllerDetectedScreen>(currentScreen))
        {
            layeredScreen->openScreen(currentScreenName);
            layeredScreen->Draw();
        }

        layeredScreen->setDirty();

        // Sometimes after a crash sounds are not loaded.
        // The below is to prevent further crashing.
        for (auto& p : mpc.getSampler()->getPrograms())
        {
            if (!p.lock())
            {
                continue;
            }
            
            for (auto& n : p.lock()->getNotesParameters())
            {
                if (n->getSoundIndex() >= mpc.getSampler()->getSoundCount())
                {
                    n->setSoundIndex(-1);
                }
            }
        }

        if (mpc.getSampler()->getProgramCount() == 0)
        {
            mpc.getSampler()->addProgram(0);
        }

        for (int i = 0; i < 4; i++)
        {
            auto d = mpc.getDrum(i);

            if (!mpc.getSampler()->getProgram(d.getProgram()))
            {
                d.setProgram(0);
            }
        }
    };

    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 1)
    {
        // ASK
        auto vmpcContinuePreviousSessionScreen = mpc.screens->get<VmpcContinuePreviousSessionScreen>();
        vmpcContinuePreviousSessionScreen->setRestoreAutoSavedStateAction(restoreAutoSavedStateAction);
        mpc.getLayeredScreen()->openScreen<VmpcContinuePreviousSessionScreen>();
        return;
    }

    // ALWAYS

    restoreAutoSavedStateAction();
}

void AutoSave::storeAutoSavedState(mpc::Mpc &mpc)
{
    auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>();

    if (vmpcAutoSaveScreen->getAutoSaveOnExit() == 0 ||
        mpc.getLayeredScreen()->getCurrentScreenName() == "vmpc-continue-previous-session")
    {
        return;
    }

    const auto path = mpc.paths->autoSavePath();
    const auto apsFile = path / "APS.APS";
    const auto allFile = path / "ALL.ALL";
    const auto soundIndexFile = path / "soundIndex.txt";
    const auto lastPressedPadFile = path / "lastPressedPad.txt";
    const auto lastPressedNoteFile = path / "lastPressedNote.txt";
    const auto screenFile = path / "screen.txt";
    const auto previousScreenFile = path / "previousScreen.txt";
    const auto previousSamplerScreenFile = path / "previousSamplerScreen.txt";
    const auto focusFile = path / "focus.txt";
    const auto soundsFile = path / "sounds.txt";
    const auto currentDirFile = path / "currentDir.txt";

    std::function<void()> storeAutoSavedStateAction = [&](){

        auto layeredScreen = mpc.getLayeredScreen();

        // Reopening the Directory window is problematic because:
        // 1. We're not storing the directory screen's x and y pos
        // 2. Even if we did, these values may be invalid due to changes
        //    in directory content.
        // So we're going for a sane default, which is to open either
        // the LOAD or SAVE screen, depending from where the user
        // opened the Directory window.
        // We do this by pressing F4 via ...->function(3)
        if (layeredScreen->getCurrentScreenName() == "directory")
        {
            mpc.getScreen()->function(3);
            layeredScreen->setPreviousScreenName("sequencer");
        }

        std::shared_ptr<ScreenComponent> currentScreen = layeredScreen->findScreenComponent();

        auto previousScreenName = layeredScreen->getPreviousScreenName();

        if (std::dynamic_pointer_cast<VmpcContinuePreviousSessionScreen>(currentScreen) ||
            std::dynamic_pointer_cast<VmpcKnownControllerDetectedScreen>(currentScreen))
        {
            currentScreen = std::dynamic_pointer_cast<ScreenComponent>(mpc.screens->get<SequencerScreen>());
        }
        else if (auto editSoundScreen = std::dynamic_pointer_cast<EditSoundScreen>(currentScreen))
        {
            currentScreen = mpc.screens->getByName1(editSoundScreen->getReturnToScreenName());
        }
        else if (std::dynamic_pointer_cast<NameScreen>(currentScreen))
        {
            currentScreen = std::dynamic_pointer_cast<ScreenComponent>(mpc.screens->get<SequencerScreen>());
            previousScreenName = "";
        }

        if (previousScreenName == "vmpc-continue-previous-session" || previousScreenName == "vmpc-known-controller-detected")
        {
            previousScreenName = "sequencer";
        }

        auto previousSamplerScreen = mpc.getPreviousSamplerScreenName();
        auto focus = mpc.getLayeredScreen()->getFocus();
        auto soundIndex = mpc.getSampler()->getSoundIndex();
        auto lastPressedPad = mpc.getPad();
        auto lastPressedNote = mpc.getNote();
        auto currentDir = mpc.getDisk()->getAbsolutePath();

        auto setFileData = [](const fs::path& p, const std::string& data) {
            if (data.empty())
            {
                fs::remove(p);
            }
            else
            {
                set_file_data(p, data);
            }
        };

        setFileData(screenFile, currentScreen->getName());
        setFileData(previousScreenFile, previousScreenName);
        setFileData(previousSamplerScreenFile, previousScreenName);
        setFileData(focusFile, focus);
        setFileData(currentDirFile, currentDir);
        setFileData(soundIndexFile, {(char)soundIndex});
        setFileData(lastPressedNoteFile, {(char)lastPressedNote});
        setFileData(lastPressedPadFile, {(char)lastPressedPad});

        {
            ApsParser apsParser(mpc, "stateinfo");
            set_file_data(apsFile, apsParser.getBytes());
        }

        auto sounds = mpc.getSampler()->getSounds();

        std::string soundNames;

        for (auto& sound : sounds)
        {
            SndWriter sndWriter(sound.get());
            auto& data = sndWriter.getSndFileArray();
            auto soundFilePath = path / (sound->getName() + ".SND");
            set_file_data(soundFilePath, data);
            soundNames = soundNames.append(sound->getName() + ".SND\n");
        }

        {
            AllParser allParser(mpc);
            set_file_data(allFile, allParser.getBytes());
        }

        setFileData(soundsFile, soundNames);
    };

    storeAutoSavedStateAction();
}
