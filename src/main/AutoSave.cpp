#include "AutoSave.hpp"

#include "Mpc.hpp"
#include "Paths.hpp"
#include "mpc_fs.hpp"

#include <disk/AbstractDisk.hpp>
#include <disk/ApsLoader.hpp>
#include <file/aps/ApsParser.hpp>
#include <file/sndreader/SndReader.hpp>
#include <file/sndwriter/SndWriter.hpp>
#include <lcdgui/screens/VmpcAutoSaveScreen.hpp>
#include <lcdgui/screens/window/VmpcContinuePreviousSessionScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>

#include "disk/AllLoader.hpp"
#include "file/all/AllParser.hpp"

#include "lang/StrUtil.hpp"

using namespace mpc;
using namespace mpc::file::all;
using namespace mpc::file::aps;
using namespace mpc::file::sndwriter;
using namespace mpc::file::sndreader;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

using namespace moduru::lang;

void AutoSave::restoreAutoSavedState(mpc::Mpc &mpc, const std::string& overridePath)
{
    auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");

    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 0)
    {
        return;
    }

    const auto path = overridePath.empty() ? Paths::autoSavePath() : fs::path(overridePath);
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
                ApsParser apsParser(mpc, data);
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
                    const auto soundPath = Paths::autoSavePath() / soundName;

                    auto soundData = get_file_data(soundPath);

                    SndReader sndReader(soundData);

                    auto sound = mpc.getSampler()->addSound(sndReader.getSampleRate());
                    sound->setMono(sndReader.isMono());
                    sndReader.readData(*sound->getSampleData());
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

        auto relativePath = fs::relative(currentDir, mpc::Paths::defaultLocalVolumePath());

        for (auto& pathSegment : relativePath)
        {
            mpc.getDisk()->moveForward(pathSegment.string());
            mpc.getDisk()->initFiles();
        }

        auto previousSamplerScreen = getStringProperty("previousSamplerScreen.txt");
        auto screen = getStringProperty("screen.txt");
        auto previousScreen = getStringProperty("previousScreen.txt");
        auto focus = getStringProperty("focus.txt");

        auto layeredScreen = mpc.getLayeredScreen();

        auto currentScreen = layeredScreen->getCurrentScreenName();

        layeredScreen->openScreen(previousSamplerScreen.empty() ? "sequencer" : previousSamplerScreen);
        layeredScreen->Draw();
        layeredScreen->openScreen(previousScreen);
        layeredScreen->Draw();

        auto directoryScreen = mpc.screens->get<DirectoryScreen>("directory");
        directoryScreen->setPreviousScreenName(previousScreen == "save" ? "save" : "load");

        layeredScreen->openScreen(screen);
        layeredScreen->Draw();

        if (!focus.empty())
        {
            layeredScreen->setFocus(focus);
        }

        if (currentScreen == "vmpc-known-controller-detected")
        {
            layeredScreen->openScreen(currentScreen);
            layeredScreen->Draw();
        }

        layeredScreen->setDirty();

        // Sometimes after a crash sounds are not loaded.
        // The below is to prevent further crashing.
        for (auto& p : mpc.getSampler()->getPrograms())
        {
            for (auto& n : p.lock()->getNotesParameters())
            {
                if (n->getSoundIndex() >= mpc.getSampler()->getSoundCount())
                {
                    n->setSoundIndex(-1);
                }
            }
        }
    };

    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 1)
    {
        // ASK
        auto vmpcContinuePreviousSessionScreen = mpc.screens->get<VmpcContinuePreviousSessionScreen>(
                "vmpc-continue-previous-session");
        vmpcContinuePreviousSessionScreen->setRestoreAutoSavedStateAction(restoreAutoSavedStateAction);
        mpc.getLayeredScreen()->openScreen("vmpc-continue-previous-session");
        return;
    }

    // ALWAYS

    restoreAutoSavedStateAction();
}

void AutoSave::storeAutoSavedState(mpc::Mpc &mpc, const std::string& overridePath)
{
    auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");

    if (vmpcAutoSaveScreen->getAutoSaveOnExit() == 0 ||
        mpc.getLayeredScreen()->getCurrentScreenName() == "vmpc-continue-previous-session")
    {
        return;
    }

    const auto path = overridePath.empty() ? Paths::autoSavePath() : fs::path(overridePath);
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
        auto screen = layeredScreen->getCurrentScreenName();
        auto previousScreen = layeredScreen->getPreviousScreenName();

        if (screen == "vmpc-continue-previous-session" || screen == "vmpc-known-controller-detected")
        {
            screen = "sequencer";
        }

        if (previousScreen == "vmpc-continue-previous-session" || previousScreen == "vmpc-known-controller-detected")
        {
            previousScreen = "sequencer";
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

        setFileData(screenFile, screen);
        setFileData(previousScreenFile, previousScreen);
        setFileData(previousSamplerScreenFile, previousScreen);
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
            auto data = sndWriter.getSndFileArray();
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