#include "AutoSave.hpp"

#include "Mpc.hpp"
#include "SaveTarget.hpp"

#include "controller/ClientEventController.hpp"

#include "mpc_fs.hpp"

#include "disk/AbstractDisk.hpp"
#include "disk/ApsLoader.hpp"
#include "FileIoPolicy.hpp"
#include "file/aps/ApsParser.hpp"
#include "file/ByteUtil.hpp"
#include "file/sndreader/SndReader.hpp"
#include "file/sndwriter/SndWriter.hpp"
#include "lcdgui/screens/VmpcAutoSaveScreen.hpp"
#include "lcdgui/screens/window/VmpcContinuePreviousSessionScreen.hpp"
#include "lcdgui/screens/window/SaveAllFileScreen.hpp"
#include "lcdgui/screens/window/SaveApsFileScreen.hpp"
#include "lcdgui/Screens.hpp"

#include "disk/AllLoader.hpp"
#include "file/all/AllParser.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

#include "StrUtil.hpp"
#include "engine/EngineHost.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "performance/PerformanceManager.hpp"
#include "sequencer/Transport.hpp"

#include <chrono>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>

using namespace mpc;
using namespace mpc::file_io;
using namespace mpc::file;
using namespace mpc::file::all;
using namespace mpc::file::aps;
using namespace mpc::file::sndwriter;
using namespace mpc::file::sndreader;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

namespace
{
bool rangeFits(const size_t totalSize, const size_t offset, const size_t length)
{
    return offset <= totalSize && length <= totalSize - offset;
}

bool isValidAutoSaveApsData(const std::vector<char> &data)
{
    if (!rangeFits(data.size(), ApsParser::HEADER_OFFSET, ApsParser::HEADER_LENGTH))
    {
        return false;
    }

    const unsigned char magic0 = static_cast<unsigned char>(data[0]);
    const unsigned char magic1 = static_cast<unsigned char>(data[1]);
    if (magic0 != 0x0A || magic1 != 0x05)
    {
        return false;
    }

    const uint16_t soundCount =
        ByteUtil::bytes2ushort({data[2], data[3]});
    if (soundCount > Mpc2000XlSpecs::MAX_SOUND_COUNT_IN_MEMORY)
    {
        return false;
    }

    const size_t minSize = static_cast<size_t>(ApsParser::HEADER_LENGTH) +
                           static_cast<size_t>(soundCount) *
                               ApsParser::SOUND_NAME_LENGTH +
                           ApsParser::PAD_LENGTH1 + ApsParser::APS_NAME_LENGTH +
                           ApsParser::PARAMETERS_LENGTH + ApsParser::TABLE_LENGTH +
                           ApsParser::PAD_LENGTH2 +
                           4u * (ApsParser::MIXER_LENGTH +
                                 ApsParser::DRUM_CONFIG_LENGTH +
                                 ApsParser::DRUM_PAD_LENGTH);

    return data.size() >= minSize;
}

bool isValidAutoSaveAllData(const std::vector<char> &data)
{
    if (data.size() < static_cast<size_t>(AllParser::SEQUENCES_OFFSET))
    {
        return false;
    }

    static const std::string expectedHeader = "MPC2KXL ALL 1.00";
    return std::equal(expectedHeader.begin(), expectedHeader.end(), data.begin());
}

bool isValidAutoSaveSndData(const std::vector<char> &data)
{
    constexpr size_t sndHeaderSize = 42;
    if (data.size() < sndHeaderSize)
    {
        return false;
    }

    const unsigned char id0 = static_cast<unsigned char>(data[0]);
    const unsigned char id1 = static_cast<unsigned char>(data[1]);
    if (id0 != 1 || id1 >= 5)
    {
        return false;
    }

    const bool mono = data[21] == 0;
    const uint32_t frameCount = ByteUtil::bytes2uint(
        {data[30], data[31], data[32], data[33]});
    const size_t channelCount = mono ? 1u : 2u;

    if (frameCount >
        std::numeric_limits<size_t>::max() / (channelCount * sizeof(int16_t)))
    {
        return false;
    }

    const size_t sampleDataBytes =
        static_cast<size_t>(frameCount) * channelCount * sizeof(int16_t);
    return rangeFits(data.size(), sndHeaderSize, sampleDataBytes);
}
} // namespace

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

    const std::vector<mpc_fs::path> files{"APS.APS",
                                      "ALL.ALL",
                                      "soundIndex.txt",
                                      "selectedPad.txt",
                                      "selectedNote.txt",
                                      "screen.txt",
                                      "focus.txt",
                                      "sounds.txt",
                                      "currentDir.txt",
                                      "last_aps_file_name.txt",
                                      "last_all_file_name.txt"};

    std::vector<mpc_fs::path> availableFiles;
    for (const auto &f : files)
    {
        const auto existsValue = value(
            saveTarget->exists(f), FailurePolicy::Required,
            "auto-save restore existence check for '" + f.string() + "'");
        if (existsValue && *existsValue)
        {
            availableFiles.push_back(f);
        }
    }

    if (availableFiles.empty())
    {
        return;
    }

    const auto restoreAction =
        [&mpc, availableFiles, saveTarget, headless, this]
    {
        try
        {
            auto layeredScreen = mpc.getLayeredScreen();
            std::map<mpc_fs::path, std::vector<char>> processInOrder;

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

                const auto sizeValue = value(
                    saveTarget->fileSize(f), FailurePolicy::Required,
                    "auto-save restore size check for '" + f.string() + "'");
                if (!sizeValue)
                {
                    continue;
                }

                if (*sizeValue == 0)
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
                else if (f == "last_all_file_name.txt")
                {
                    desc = "last ALL file name";
                }
                else if (f == "last_aps_file_name.txt")
                {
                    desc = "last APS file name";
                }

                showMsg("Loading " + desc);

                const auto dataValue = value(
                    saveTarget->getFileData(f), FailurePolicy::Required,
                    "auto-save restore read for '" + f.string() + "'");
                if (!dataValue)
                {
                    continue;
                }
                const auto &data = *dataValue;

                try
                {
                    if (f == "APS.APS")
                    {
                        if (!isValidAutoSaveApsData(data))
                        {
                            throw std::invalid_argument("invalid APS autosave data");
                        }
                        ApsParser apsParser(data);
                        constexpr bool withoutSounds = true;
                        // Regardless of whether the outer context is headless,
                        // we want the ApsLoader to always operate in headless
                        // mode so it doesn't spawn popups. We manage popups
                        // ourselves here in AutoSave.
                        constexpr bool apsLoadingHeadless = true;
                        disk::ApsLoader::loadFromParsedAps(
                            apsParser, mpc, withoutSounds, apsLoadingHeadless);
                    }
                    else if (f == "ALL.ALL")
                    {
                        if (!isValidAutoSaveAllData(data))
                        {
                            throw std::invalid_argument("invalid ALL autosave data");
                        }
                        AllParser allParser(mpc, data);
                        disk::AllLoader::loadEverythingFromAllParser(mpc, allParser);
                    }
                    else if (f == "sounds.txt")
                    {
                        auto soundNames = StrUtil::split(
                            std::string(data.begin(), data.end()), '\n');
                        for (auto &soundName : soundNames)
                        {
                            if (soundName.empty())
                            {
                                continue;
                            }

                            showMsg("Loading " + soundName.substr(0, 20));

                            const auto soundDataValue = value(
                                saveTarget->getFileData(soundName),
                                FailurePolicy::Required,
                                "auto-save restore sound read for '" +
                                    soundName + "'");
                            if (!soundDataValue)
                            {
                                continue;
                            }
                            const auto &soundData = *soundDataValue;
                            if (!isValidAutoSaveSndData(soundData))
                            {
                                MLOG("AutoSave restore skipped corrupt file '" +
                                     soundName + "'");
                                continue;
                            }
                            SndReader sndReader(soundData);
                            auto sound = mpc.getSampler()->addSound(
                                sndReader.getSampleRate());
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
                catch (const std::exception &e)
                {
                    MLOG("AutoSave restore skipped corrupt file '" + f.string() +
                         "': " + std::string(e.what()));
                }
                catch (...)
                {
                    MLOG("AutoSave restore skipped corrupt file '" + f.string() +
                         "': unknown exception");
                }
            }

            const auto setIntProperty = [&](const std::string &file,
                                            const std::function<void(int)> &setter)
            {
                if (processInOrder.count(file) && !processInOrder[file].empty())
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

            auto currentDir = mpc_fs::path(getStringProperty("currentDir.txt"));
            auto relativePath = mpc_fs::relative(
                currentDir, mpc.paths->getDocuments()->defaultLocalVolumePath());
            if (!relativePath)
            {
                return;
            }
            for (auto &seg : *relativePath)
            {
                mpc.getDisk()->moveForward(seg.string());
                mpc.getDisk()->initFiles();
            }

            const auto screenName = getStringProperty("screen.txt");
            const auto focusName = getStringProperty("focus.txt");

            const auto lastAllFileName =
                getStringProperty("last_all_file_name.txt");

            const auto lastApsFileName =
                getStringProperty("last_aps_file_name.txt");

            utils::Task uiTask1(
                [screens = mpc.screens, lastAllFileName, lastApsFileName]
                {
                    if (!lastAllFileName.empty())
                    {
                        const auto saveAllFileScreen =
                            screens->get<ScreenId::SaveAllFileScreen>();

                        saveAllFileScreen->setFileName(lastAllFileName);
                    }

                    if (!lastApsFileName.empty())
                    {
                        const auto saveApsFileScreen =
                            screens->get<ScreenId::SaveApsFileScreen>();

                        saveApsFileScreen->setFileName(lastApsFileName);
                    }
                });

            utils::Task uiTask2(
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

            layeredScreen->postToUiThread(std::move(uiTask1));
            layeredScreen->postToUiThread(std::move(uiTask2));

            for (auto &p : mpc.getSampler()->getPrograms())
            {
                if (auto pl = p.lock())
                {
                    for (auto &n : pl->getNotesParameters())
                    {
                        if (n->getSoundIndex() >=
                            mpc.getSampler()->getSoundCount())
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

            utils::SimpleAction action(
                [&mpc]
                {
                    for (size_t drumBusIndex = 0;
                         drumBusIndex < Mpc2000XlSpecs::DRUM_BUS_COUNT;
                         ++drumBusIndex)
                    {
                        if (const auto d = mpc.getSequencer()->getDrumBus(
                                sequencer::drumBusIndexToDrumBusType(
                                    drumBusIndex));
                            !mpc.getSampler()
                                 ->getProgram(d->getProgramIndex())
                                 ->isUsed())
                        {
                            d->setProgramIndex(ProgramIndex{0});
                        }
                    }
                });

            mpc.getPerformanceManager().lock()->enqueueCallback(
                std::move(action));
        }
        catch (const std::exception &e)
        {
            MLOG("AutoSave restore aborted: " + std::string(e.what()));
        }
        catch (...)
        {
            MLOG("AutoSave restore aborted: unknown exception");
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

        if (focus == "nextsq") focus = "sq";

        auto soundIndex = mpc.getSampler()->getSoundIndex();
        auto selectedPad = mpc.clientEventController->getSelectedPad();
        auto selectedNote = mpc.clientEventController->getSelectedNote();
        auto currentDir = mpc.getDisk()->getAbsolutePath();

        const auto saveAllFileScreen =
            mpc.screens->get<ScreenId::SaveAllFileScreen>();

        const auto saveApsFileScreen =
            mpc.screens->get<ScreenId::SaveApsFileScreen>();

        const auto lastAllFileName = saveAllFileScreen->getFileName();
        const auto lastApsFileName = saveApsFileScreen->getFileName();

        auto writeRequired = [&](const mpc_fs::path &path,
                                 const std::vector<char> &data)
        {
            return success(saveTarget->setFileData(path, data),
                           FailurePolicy::Required,
                           "auto-save store write for '" + path.string() + "'");
        };

        if (!writeRequired("screen.txt", {currentScreen.begin(), currentScreen.end()}) ||
            !writeRequired("last_all_file_name.txt",
                           {lastAllFileName.begin(), lastAllFileName.end()}) ||
            !writeRequired("last_aps_file_name.txt",
                           {lastApsFileName.begin(), lastApsFileName.end()}) ||
            !writeRequired("focus.txt", {focus.begin(), focus.end()}) ||
            !writeRequired("currentDir.txt", {currentDir.begin(), currentDir.end()}) ||
            !writeRequired("soundIndex.txt", {static_cast<char>(soundIndex)}) ||
            !writeRequired("selectedNote.txt", {static_cast<char>(selectedNote)}) ||
            !writeRequired("selectedPad.txt", {static_cast<char>(selectedPad)}))
        {
            layeredScreen->closeCurrentScreen();
            return;
        }

        {
            ApsParser apsParser(mpc, "stateinfo");
            if (!writeRequired("APS.APS", apsParser.getBytes()))
            {
                layeredScreen->closeCurrentScreen();
                return;
            }
        }

        std::string soundNames;
        for (auto &sound : mpc.getSampler()->getSounds())
        {
            const auto sndPath = sound->getName() + ".SND";
            SndWriter sndWriter(sound.get());
            if (!writeRequired(sndPath,
                               {sndWriter.getSndFileArray().begin(),
                                sndWriter.getSndFileArray().end()}))
            {
                layeredScreen->closeCurrentScreen();
                return;
            }
            soundNames += sndPath + "\n";
        }

        {
            AllParser allParser(mpc);
            if (!writeRequired("ALL.ALL", allParser.getBytes()))
            {
                layeredScreen->closeCurrentScreen();
                return;
            }
        }

        if (!writeRequired("sounds.txt", {soundNames.begin(), soundNames.end()}))
        {
            layeredScreen->closeCurrentScreen();
            return;
        }
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
