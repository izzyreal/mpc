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

#include "file/FileUtil.hpp"
#include "lang/StrUtil.hpp"

using namespace mpc;
using namespace mpc::file::all;
using namespace mpc::file::aps;
using namespace mpc::file::sndwriter;
using namespace mpc::file::sndreader;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

using namespace moduru::file;
using namespace moduru::lang;

void AutoSave::restoreAutoSavedState(mpc::Mpc &mpc, const std::string& overridePath)
{
    auto vmpcAutoSaveScreen = mpc.screens->get<VmpcAutoSaveScreen>("vmpc-auto-save");

    if (vmpcAutoSaveScreen->getAutoLoadOnStart() == 0)
    {
        return;
    }

    const auto path = overridePath.empty() ? Paths::autoSavePath() : overridePath;
    const auto apsFile = path + "APS.APS";
    const auto allFile = path + "ALL.ALL";
    const auto soundIndexFile = path + "soundIndex.txt";
    const auto lastPressedPadFile = path + "lastPressedPad.txt";
    const auto lastPressedNoteFile = path + "lastPressedNote.txt";
    const auto screenFile = path + "screen.txt";
    const auto previousScreenFile = path + "previousScreen.txt";
    const auto previousSamplerScreenFile = path + "previousSamplerScreen.txt";
    const auto focusFile = path + "focus.txt";
    const auto soundsFile = path + "sounds.txt";
    const auto currentDirFile = path + "currentDir.txt";

    std::vector<std::string> files{apsFile, allFile, soundIndexFile, lastPressedPadFile, lastPressedNoteFile, screenFile, previousScreenFile, previousSamplerScreenFile, focusFile, soundsFile, currentDirFile };

    std::vector<std::string> availableFiles;

    for (auto &f: files)
    {
        auto fp = fs::path(f);

        if (fs::exists(fp))
        {
            availableFiles.push_back(f);
        }
    }

    if (availableFiles.empty())
    {
        return;
    }

    const auto restoreAutoSavedStateAction = [&mpc, availableFiles, path] {

        std::map<std::string, std::vector<char>> processInOrder;

        for (auto &f: availableFiles)
        {
            auto stream = FileUtil::ifstreamw(f, std::ios::in | std::ios::binary);
            auto size1 = fs::file_size(f);

            if (size1 == 0)
            {
                continue;
            }

            std::vector<char> data(size1);
            stream.read(&data[0], size1);
            stream.close();

            if (f == path + "APS.APS")
            {
                ApsParser apsParser(mpc, data);
                disk::ApsLoader::loadFromParsedAps(apsParser, mpc, true, true);
            }
            else if (f == path + "ALL.ALL")
            {
                AllParser allParser(mpc, data);
                disk::AllLoader::loadEverythingFromAllParser(mpc, allParser);
            }
            else if (f == path + "sounds.txt")
            {
                auto soundNames = StrUtil::split(std::string(data.begin(), data.end()), '\n');

                for (auto &soundName: soundNames)
                {
                    auto soundPath = Paths::autoSavePath() + soundName;
                    auto soundStream = FileUtil::ifstreamw(soundPath, std::ios::in | std::ios::binary);
                    auto size = fs::file_size(soundPath);
                    std::vector<char> soundData(size);
                    soundStream.read(&soundData[0], size);
                    soundStream.close();

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

        const auto setIntProperty = [&processInOrder, path](std::string prop, std::function<void(int v)> setter) {
            if (processInOrder.find(path + prop) != processInOrder.end())
            {
                std::vector<char> str = processInOrder[path + prop];
                try
                {
                    auto value = str[0];
                    setter(value);
                } catch (const std::exception &)
                {}
            }
        };

        const auto getStringProperty = [&processInOrder, path](std::string prop) -> std::string {
            if (processInOrder.find(path + prop) != processInOrder.end())
            {
                return std::string(processInOrder[path + prop].begin(), processInOrder[path + prop].end());
            } else return {};
        };

        setIntProperty("soundIndex.txt", [&mpc](int v){mpc.getSampler()->setSoundIndex(v);});
        setIntProperty("lastPressedNote.txt", [&mpc](int v){mpc.setNote(v);});
        setIntProperty("lastPressedPad.txt", [&mpc](int v){mpc.setPad(v);});

        auto currentDir = getStringProperty("currentDir.txt");

        auto storesPath = mpc::Paths::storesPath() + "MPC2000XL";
        auto resPathIndex = currentDir.find(storesPath);

        if (resPathIndex != std::string::npos)
        {
            auto trimmedCurrentDir = currentDir.substr(resPathIndex + storesPath.length());
            auto splitTrimmedDir = StrUtil::split(trimmedCurrentDir, FileUtil::getSeparator()[0]);

            for (auto& s : splitTrimmedDir)
            {
                mpc.getDisk()->moveForward(s);
                mpc.getDisk()->initFiles();
            }
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

    if (vmpcAutoSaveScreen->getAutoSaveOnExit() == 0)
    {
        return;
    }

    const auto path = overridePath.empty() ? Paths::autoSavePath() : overridePath;
    const auto apsFile = path + "APS.APS";
    const auto allFile = path + "ALL.ALL";
    const auto soundIndexFile = path + "soundIndex.txt";
    const auto lastPressedPadFile = path + "lastPressedPad.txt";
    const auto lastPressedNoteFile = path + "lastPressedNote.txt";
    const auto screenFile = path + "screen.txt";
    const auto previousScreenFile = path + "previousScreen.txt";
    const auto previousSamplerScreenFile = path + "previousSamplerScreen.txt";
    const auto focusFile = path + "focus.txt";
    const auto soundsFile = path + "sounds.txt";
    const auto currentDirFile = path + "currentDir.txt";

    std::function<void()> storeAutoSavedStateAction = [&](){

        auto layeredScreen = mpc.getLayeredScreen();
        auto screen = layeredScreen->getCurrentScreenName();
        auto previousScreen = layeredScreen->getPreviousScreenName();

        if (screen == "vmpc-continue-previous-session")
        {
            screen = "sequencer";
        }

        if (previousScreen == "vmpc-continue-previous-session")
        {
            previousScreen = "sequencer";
        }

        auto previousSamplerScreen = mpc.getPreviousSamplerScreenName();
        auto focus = mpc.getLayeredScreen()->getFocus();
        auto soundIndex = mpc.getSampler()->getSoundIndex();
        auto lastPressedPad = mpc.getPad();
        auto lastPressedNote = mpc.getNote();
        auto currentDir = mpc.getDisk()->getAbsolutePath();

        if (!screen.empty())
        {
            auto stream = FileUtil::ofstreamw(screenFile, std::ios::out | std::ios::binary);
            stream.write(&screen[0], screen.length());
            stream.close();
        }

        if (!previousScreen.empty())
        {
            auto stream = FileUtil::ofstreamw(previousScreenFile, std::ios::out | std::ios::binary);
            stream.write(&previousScreen[0], previousScreen.length());
            stream.close();
        }

        if (!previousSamplerScreen.empty())
        {
            auto stream = FileUtil::ofstreamw(previousSamplerScreenFile, std::ios::out | std::ios::binary);
            stream.write(&previousSamplerScreen[0], previousSamplerScreen.length());
            stream.close();
        }

        if (!focus.empty())
        {
            auto stream = FileUtil::ofstreamw(focusFile, std::ios::out | std::ios::binary);
            stream.write(&focus[0], focus.length());
            stream.close();
        }

        if (!currentDir.empty())
        {
            auto stream = FileUtil::ofstreamw(currentDirFile, std::ios::out | std::ios::binary);
            stream.write(&currentDir[0], currentDir.length());
            stream.close();
        }

        {
            auto stream = FileUtil::ofstreamw(soundIndexFile, std::ios::out | std::ios::binary);
            stream.put(static_cast<char>(soundIndex));
            stream.close();
        }

        {
            auto stream = FileUtil::ofstreamw(lastPressedNoteFile, std::ios::out | std::ios::binary);
            stream.put(static_cast<char>(lastPressedNote));
            stream.close();
        }

        {
            auto stream = FileUtil::ofstreamw(lastPressedPadFile, std::ios::out | std::ios::binary);
            stream.put(static_cast<char>(lastPressedPad));
            stream.close();
        }


        {
            ApsParser apsParser(mpc, "stateinfo");
            auto apsBytes = apsParser.getBytes();

            auto stream = FileUtil::ofstreamw(apsFile, std::ios::out | std::ios::binary);
            stream.write(&apsBytes[0], apsBytes.size());
            stream.close();
        }

        auto sounds = mpc.getSampler()->getSounds();

        std::string soundNames;

        for (auto& sound : sounds)
        {
            SndWriter sndWriter(sound.get());
            auto data = sndWriter.getSndFileArray();
            auto soundFilePath = path + sound->getName() + ".SND";
            auto stream = FileUtil::ofstreamw(soundFilePath, std::ios::out | std::ios::binary);
            stream.write(&data[0], (long) data.size());
            stream.close();
            soundNames = soundNames.append(sound->getName() + ".SND\n");
        }

        {
            AllParser allParser(mpc);
            auto allBytes = allParser.getBytes();

            auto stream = FileUtil::ofstreamw(allFile, std::ios::out | std::ios::binary);
            stream.write(&allBytes[0], allBytes.size());
            stream.close();
        }

        if (!soundNames.empty()){
            auto stream = FileUtil::ofstreamw(soundsFile, std::ios::out | std::ios::binary);
            stream.write(&soundNames[0], soundNames.size());
            stream.close();
        }
    };

    storeAutoSavedStateAction();
}