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
    const auto focusFile = path / "focus.txt";
    const auto soundsFile = path / "sounds.txt";
    const auto currentDirFile = path / "currentDir.txt";

    std::vector<fs::path> files{apsFile, allFile, soundIndexFile, lastPressedPadFile, lastPressedNoteFile, screenFile, focusFile, soundsFile, currentDirFile};

    std::vector<fs::path> availableFiles;

    for (auto &f : files)
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

    const auto restoreAutoSavedStateAction = [&mpc, availableFiles, path]
    {
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

                for (auto &soundName : soundNames)
                {
                    const auto soundPath = mpc.paths->autoSavePath() / soundName;

                    auto soundData = get_file_data(soundPath);

                    SndReader sndReader(soundData);

                    auto sound = mpc.getSampler()->addSound(sndReader.getSampleRate());

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

        const auto setIntProperty = [&processInOrder, path](const std::string &prop, const std::function<void(int v)> &setter)
        {
            if (processInOrder.find(path / prop) != processInOrder.end())
            {
                std::vector<char> str = processInOrder[path / prop];
                try
                {
                    auto value = str[0];
                    setter(value);
                }
                catch (const std::exception &)
                {
                }
            }
        };

        const auto getStringProperty = [&processInOrder, path](const std::string &prop) -> std::string
        {
            if (processInOrder.find(path / prop) != processInOrder.end())
            {
                return {processInOrder[path / prop].begin(), processInOrder[path / prop].end()};
            }
            else
            {
                return {};
            }
        };

        setIntProperty("soundIndex.txt", [&mpc](int v)
                       {
                           mpc.getSampler()->setSoundIndex(v);
                       });
        setIntProperty("lastPressedNote.txt", [&mpc](int v)
                       {
                           mpc.clientEventController->setSelectedNote(v);
                       });
        setIntProperty("lastPressedPad.txt", [&mpc](int v)
                       {
                           mpc.clientEventController->setSelectedPad(v);
                       });

        auto currentDir = fs::path(getStringProperty("currentDir.txt"));

        auto relativePath = fs::relative(currentDir, mpc.paths->defaultLocalVolumePath());

        for (auto &pathSegment : relativePath)
        {
            mpc.getDisk()->moveForward(pathSegment.string());
            mpc.getDisk()->initFiles();
        }

        const auto screenName = getStringProperty("screen.txt");

        const auto focusName = getStringProperty("focus.txt");

        const auto layeredScreen = mpc.getLayeredScreen();

        layeredScreen->openScreen(screenName);

        if (!focusName.empty())
        {
            layeredScreen->setFocus(focusName);
        }

        // Sometimes after a crash sounds are not loaded.
        // The below is to prevent further crashing.
        for (auto &p : mpc.getSampler()->getPrograms())
        {
            if (!p.lock())
            {
                continue;
            }

            for (auto &n : p.lock()->getNotesParameters())
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
    const auto focusFile = path / "focus.txt";
    const auto soundsFile = path / "sounds.txt";
    const auto currentDirFile = path / "currentDir.txt";

    std::function<void()> storeAutoSavedStateAction = [&]()
    {
        auto layeredScreen = mpc.getLayeredScreen();

        std::string currentScreen = layeredScreen->getFirstLayerScreenName();

        auto focus = mpc.getLayeredScreen()->getFocusedFieldName();
        auto soundIndex = mpc.getSampler()->getSoundIndex();
        auto lastPressedPad = mpc.clientEventController->getSelectedPad();
        auto lastPressedNote = mpc.clientEventController->getSelectedNote();
        auto currentDir = mpc.getDisk()->getAbsolutePath();

        auto setFileData = [](const fs::path &p, const std::string &data)
        {
            if (data.empty())
            {
                fs::remove(p);
            }
            else
            {
                set_file_data(p, data);
            }
        };

        setFileData(screenFile, currentScreen);
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

        for (auto &sound : sounds)
        {
            SndWriter sndWriter(sound.get());
            auto &data = sndWriter.getSndFileArray();
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
