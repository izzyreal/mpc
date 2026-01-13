#include "AbstractDisk.hpp"

#include "Mpc.hpp"
#include "Util.hpp"

#include "disk/MpcFile.hpp"
#include "disk/ApsLoader.hpp"
#include "disk/ProgramLoader.hpp"

#include "file/wav/WavFile.hpp"
#include "file/mid/MidiWriter.hpp"
#include "file/mid/MidiReader.hpp"
#include "file/pgmwriter/PgmWriter.hpp"
#include "file/sndwriter/SndWriter.hpp"
#include "file/sndreader/SndReader.hpp"
#include "file/aps/ApsParser.hpp"
#include "file/all/AllParser.hpp"

#include "sampler/Sampler.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sound.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/SaveAProgramScreen.hpp"

#include "input/midi/MidiControlPresetV3.hpp"
#include "input/midi/MidiControlPresetUtil.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetV2Convertor.hpp"
#include "input/midi/legacy/LegacyMidiControlPresetV1Convertor.hpp"

#include "StrUtil.hpp"
#include "Logger.hpp"

#include <stdexcept>

using namespace mpc::disk;

using namespace mpc::input::midi;

using namespace mpc::file;
using namespace mpc::file::wav;
using namespace mpc::file::sndwriter;
using namespace mpc::file::sndreader;
using namespace mpc::file::mid;
using namespace mpc::file::pgmwriter;
using namespace mpc::file::aps;
using namespace mpc::file::all;

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::sampler;

AbstractDisk::AbstractDisk(Mpc &_mpc) : mpc(_mpc) {}

AbstractDisk::~AbstractDisk()
{
    if (programSoundsSaveThread.joinable())
    {
        programSoundsSaveThread.join();
    }
    if (readApsThread.joinable())
    {
        readApsThread.join();
    }
    if (readPgmThread.joinable())
    {
        readPgmThread.join();
    }
}

std::shared_ptr<MpcFile> AbstractDisk::getFile(const int fileIndex)
{
    if (fileIndex >= files.size())
    {
        return {};
    }

    return files[fileIndex];
}

std::vector<std::string> AbstractDisk::getFileNames()
{
    std::vector<std::string> res;
    transform(files.begin(), files.end(), back_inserter(res),
              [](const std::shared_ptr<MpcFile> &f)
              {
                  return f->getName();
              });
    return res;
}

std::string AbstractDisk::getFileName(const int i) const
{
    return files[i]->getName();
}

std::vector<std::string> AbstractDisk::getParentFileNames() const
{
    std::vector<std::string> res;

    for (auto &f : parentFiles)
    {
        res.push_back(f->getName().length() < 8 ? f->getName()
                                                : f->getName().substr(0, 8));
    }

    return res;
}

bool AbstractDisk::deleteSelectedFile() const
{
    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    return files[loadScreen->fileLoad]->del();
}

std::vector<std::shared_ptr<MpcFile>> &AbstractDisk::getAllFiles()
{
    return allFiles;
}

std::shared_ptr<MpcFile> AbstractDisk::getParentFile(const int i)
{
    return parentFiles[i];
}

void AbstractDisk::writeSnd(const std::shared_ptr<Sound> &s,
                            const std::string &fileName)
{
    const std::function<file_or_error()> writeFunc = [&]
    {
        const auto name = Util::getFileName(
            fileName == "" ? s->getName() + ".SND" : fileName);
        auto f = newFile(name);
        auto sw = SndWriter(s.get());
        auto &sndArray = sw.getSndFileArray();
        f->setFileData(sndArray);
        flush();
        initFiles();
        return f;
    };

    performIoOrOpenErrorPopupNonReturning(writeFunc);
}

void AbstractDisk::writeWav(const std::shared_ptr<Sound> &s,
                            const std::string &fileName)
{
    const std::function<file_or_error()> writeFunc = [&]
    {
        const auto name = Util::getFileName(
            fileName == "" ? s->getName() + ".WAV" : fileName);
        auto f = newFile(name);
        const auto outputStream = f->getOutputStream();
        const auto isMono = s->isMono();
        const auto data = s->getSampleData();
        auto wavFile = WavFile::writeWavStream(outputStream, isMono ? 1 : 2,
                                               data->size() / (isMono ? 1 : 2),
                                               16, s->getSampleRate());

        if (isMono)
        {
            wavFile.writeFrames(*data, data->size());
        }
        else
        {
            std::vector<float> interleaved;

            for (int i = 0; i < static_cast<int>(data->size() * 0.5); i++)
            {
                interleaved.push_back((*data)[i]);
                interleaved.push_back(
                    (*data)[static_cast<int>(i + data->size() * 0.5)]);
            }

            wavFile.writeFrames(interleaved, data->size() * 0.5);
        }

        wavFile.close();
        flush();
        initFiles();

        return f;
    };

    performIoOrOpenErrorPopupNonReturning(writeFunc);
}

void AbstractDisk::writeMid(const std::shared_ptr<sequencer::Sequence> &s,
                            const std::string &fileName)
{
    const std::function<file_or_error()> writeFunc = [&]
    {
        auto f = newFile(fileName);
        const MidiWriter writer(s.get());
        writer.writeToOStream(f->getOutputStream());
        flush();
        initFiles();
        mpc.getLayeredScreen()->showPopupAndThenReturnToLayer(
            "Saving" + fileName, 400, 0);
        return f;
    };

    performIoOrOpenErrorPopupNonReturning(writeFunc);
}

bool AbstractDisk::checkExists(const std::string &fileName)
{
    initFiles();

    const auto path = fs::path(fileName);

    return std::any_of(allFiles.begin(), allFiles.end(),
                       [path](const std::shared_ptr<MpcFile> &file)
                       {
                           const auto path2 = fs::path(file->getName());
                           const auto nameIsSame = StrUtil::eqIgnoreCase(
                               path2.stem().string(), path.stem().string());
                           const auto extIsSame =
                               StrUtil::eqIgnoreCase(path2.extension().string(),
                                                     path.extension().string());
                           return nameIsSame && extIsSame;
                       });
}

std::shared_ptr<MpcFile> AbstractDisk::getFile(const std::string &fileName)
{
    const auto tempfileName = StrUtil::replaceAll(fileName, ' ', "");

    for (auto &f : files)
    {
        if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""),
                                  tempfileName))
        {
            return f;
        }
    }

    for (auto &f : allFiles)
    {
        if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""),
                                  tempfileName))
        {
            return f;
        }
    }

    return {};
}

bool AbstractDisk::isRoot()
{
    return getPathDepth() == 0;
}

bool AbstractDisk::deleteRecursive(const std::weak_ptr<MpcFile> _toDelete)
{
    const auto toDelete = _toDelete.lock();

    if (toDelete->isDirectory())
    {
        for (auto &f : toDelete->listFiles())
        {
            if (f->getName() == "" || f->getName() == "." ||
                f->getName() == "..")
            {
                continue;
            }

            deleteRecursive(f);
        }
    }

    return toDelete->del();
}

void AbstractDisk::writePgm(const std::shared_ptr<Program> &p,
                            const std::string &fileName)
{
    const std::function<file_or_error()> writeFunc = [&]
    {
        auto f = newFile(fileName);
        const PgmWriter writer(p.get(), mpc.getSampler());
        auto bytes = writer.get();
        f->setFileData(bytes);

        const std::string popupMsg = "Saving " + fileName;

        const auto saveAProgramScreen =
            mpc.screens->get<ScreenId::SaveAProgramScreen>();

        if (saveAProgramScreen->save != 0)
        {
            std::vector<std::shared_ptr<Sound>> sounds;

            for (const auto &n : p->getNotesParameters())
            {
                if (const auto soundIndex = n->getSoundIndex();
                    soundIndex != -1)
                {
                    sounds.push_back(mpc.getSampler()->getSound(soundIndex));
                }
            }

            if (!sounds.empty())
            {
                if (programSoundsSaveThread.joinable())
                {
                    programSoundsSaveThread.join();
                }

                programSoundsSaveThread = std::thread(
                    [this, isWav = saveAProgramScreen->save == 2, sounds]
                    {
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(700));
                        soundSaver =
                            std::make_unique<SoundSaver>(mpc, sounds, isWav);
                    });
            }
            else
            {
                mpc.getLayeredScreen()->showPopupAndThenReturnToLayer(popupMsg,
                                                                      700, 0);
            }
        }
        else
        {
            mpc.getLayeredScreen()->showPopupAndThenReturnToLayer(popupMsg, 700,
                                                                  0);
        }

        flush();
        initFiles();
        return f;
    };

    performIoOrOpenErrorPopupNonReturning(writeFunc);
}

void AbstractDisk::writeAps(const std::string &fileName)
{
    const std::function<file_or_error()> writeFunc = [&]
    {
        auto f = newFile(fileName);
        const auto apsName = f->getNameWithoutExtension();
        ApsParser apsParser(mpc, apsName);
        auto bytes = apsParser.getBytes();
        f->setFileData(bytes);

        auto saveAProgramScreen =
            mpc.screens->get<ScreenId::SaveAProgramScreen>();

        if (saveAProgramScreen->save != 0 &&
            mpc.getSampler()->getSoundCount() > 0)
        {
            if (programSoundsSaveThread.joinable())
            {
                programSoundsSaveThread.join();
            }

            programSoundsSaveThread = std::thread(
                [this, saveAProgramScreen]
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(700));
                    soundSaver = std::make_unique<SoundSaver>(
                        mpc, mpc.getSampler()->getSounds(),
                        saveAProgramScreen->save == 2);
                });
        }
        else
        {
            const std::string popupMsg = "Saving " + fileName;
            mpc.getLayeredScreen()->showPopupAndThenReturnToLayer(popupMsg, 700,
                                                                  0);
        }

        flush();
        initFiles();

        return f;
    };

    performIoOrOpenErrorPopupNonReturning(writeFunc);
}

void AbstractDisk::writeAll(const std::string &fileName)
{
    const std::function<file_or_error()> writeFunc = [&]
    {
        auto f = newFile(fileName);
        AllParser allParser(mpc);
        auto bytes = allParser.getBytes();
        f->setFileData(bytes);

        flush();
        initFiles();

        mpc.getLayeredScreen()->showPopupAndThenReturnToLayer(
            "         Saving ...", 400, 0);
        return f;
    };

    performIoOrOpenErrorPopupNonReturning(writeFunc);
}

void AbstractDisk::writeMidiControlPreset(
    std::shared_ptr<MidiControlPresetV3> preset)
{
    const std::function<preset_or_error()> ioFunc = [preset, this]
    {
        json fileData;
        to_json(fileData, *preset);

        const auto presetPath =
            mpc.paths->getDocuments()->midiControlPresetsPath() /
            (preset->name + ".json");

        set_file_data(presetPath, fileData.dump(4));

        return preset;
    };

    performIoOrOpenErrorPopupNonReturning(ioFunc);
}

void AbstractDisk::readMidiControlPreset(
    const fs::path &p, const std::shared_ptr<MidiControlPresetV3> &preset)
{
    MLOG("Trying to read MIDI control preset at path " + p.string());

    const std::function ioFunc = [p, preset]() -> preset_or_error
    {
        auto pathToUse = p;

        if (!fs::exists(p))
        {
            if (p.extension().string().find("json") != std::string::npos)
            {
                pathToUse.replace_extension("vmp");
            }
            else if (p.extension().string().find("vmp") != std::string::npos)
            {
                pathToUse.replace_extension("json");
            }
        }

        if (!fs::exists(pathToUse))
        {
            return tl::make_unexpected(mpc_io_error_msg{"File does not exist"});
        }

        const auto fileData = get_file_data(pathToUse);

        json fileAsJson;

        bool success = false;

        if (pathToUse.extension().string().find("vmp") != std::string::npos)
        {
            bool shouldTryV1Parser = true;

            try
            {
                fileAsJson = legacy::parseLegacyMidiControlPresetV2(
                    std::string(fileData.begin(), fileData.end()));
                shouldTryV1Parser = false;
                success = true;
            }
            catch (const std::exception &)
            {
            }

            if (shouldTryV1Parser)
            {
                try
                {
                    fileAsJson = legacy::parseLegacyMidiControlPresetV1(
                        std::string(fileData.begin(), fileData.end()));
                    success = true;
                }
                catch (const std::exception &)
                {
                }
            }
        }
        else if (pathToUse.extension().string().find("json") !=
                 std::string::npos)
        {
            fileAsJson = json::parse(fileData);
            success = true;
        }

        if (success)
        {
            try
            {
                from_json(fileAsJson, *preset);
            }
            catch (const std::exception)
            {
                return tl::make_unexpected(
                    mpc_io_error_msg{"Unable to convert JSON to preset"});
            }

            MidiControlPresetUtil::ensurePresetHasAllAvailableTargets(preset);
            MidiControlPresetUtil::ensureTargetsAreInSameOrderAsInSchema(
                preset);
            return preset;
        }

        return tl::make_unexpected(
            mpc_io_error_msg{"Unable to read MIDI control preset"});
    };

    performIoOrOpenErrorPopupNonReturning(ioFunc);
}

wav_or_error AbstractDisk::readWavMeta(std::shared_ptr<MpcFile> f)
{
    const std::function readFunc = [f]
    {
        return WavFile::readWavStream(f->getInputStream());
    };

    return performIoOrOpenErrorPopup(readFunc);
}

sound_or_error AbstractDisk::readWav2(
    std::shared_ptr<MpcFile> f,
    std::function<sound_or_error(std::shared_ptr<WavFile>)> onSuccess)
{
    const std::function readFunc = [f, onSuccess]
    {
        return WavFile::readWavStream(f->getInputStream()).and_then(onSuccess);
    };
    return performIoOrOpenErrorPopup(readFunc);
}

sound_or_error AbstractDisk::readSnd2(
    std::shared_ptr<MpcFile> f,
    std::function<sound_or_error(std::shared_ptr<SndReader>)> onSuccess)
{
    const std::function readFunc = [f, onSuccess]
    {
        return onSuccess(std::make_shared<SndReader>(f.get()));
    };
    return performIoOrOpenErrorPopup(readFunc);
}

sequence_or_error AbstractDisk::readMid2(std::shared_ptr<MpcFile> f)
{
    const std::function readFunc = [this, f]() -> sequence_or_error
    {
        if (!f)
        {
            return tl::make_unexpected(mpc_io_error_msg{"Empty file"});
        }

        const auto fStream = f->getInputStream();
        auto newSeq = mpc.getSequencer()->getSequence(TempSequenceIndex);

        newSeq->init(0);

        const MidiReader midiReader(fStream, newSeq);
        midiReader.parseSequence(mpc);

        if (newSeq->getName().empty())
        {
            newSeq->setName(f->getNameWithoutExtension());
        }

        return newSeq;
    };

    return performIoOrOpenErrorPopup(readFunc);
}

void AbstractDisk::readPgm2(std::shared_ptr<MpcFile> f,
                            std::shared_ptr<Program> p, int programIndex)
{
    if (readPgmThread.joinable())
    {
        readPgmThread.join();
    }
    readPgmThread = std::thread(
        [this, f, p, programIndex]
        {
            const std::function<tl::expected<bool, mpc_io_error_msg>()>
                readFunc = [this, f, p, programIndex]
            {
                (void)ProgramLoader::loadProgram(mpc, f, p, programIndex);
                return true;
            };

            performIoOrOpenErrorPopupNonReturning(readFunc);
        });
}

void AbstractDisk::readAps2(std::shared_ptr<MpcFile> f,
                            std::function<void()> onSuccess, bool headless)
{
    if (readApsThread.joinable())
    {
        readApsThread.join();
    }
    readApsThread = std::thread(
        [this, f, onSuccess, headless]
        {
            const std::function<tl::expected<bool, mpc_io_error_msg>()>
                readFunc = [this, f, onSuccess, headless]
            {
                ApsLoader::load(mpc, f, headless);
                onSuccess();
                return true;
            };

            performIoOrOpenErrorPopupNonReturning(readFunc);
        });
}

void AbstractDisk::readAll2(std::shared_ptr<MpcFile> f,
                            std::function<void()> onSuccess)
{
    const std::function<tl::expected<bool, mpc_io_error_msg>()> readFunc =
        [this, f, onSuccess]
    {
        AllLoader::loadEverythingFromFile(mpc, f.get());
        onSuccess();
        return true;
    };

    performIoOrOpenErrorPopupNonReturning(readFunc);
}

sequence_meta_infos_or_error
AbstractDisk::readSequenceMetaInfosFromAllFile(std::shared_ptr<MpcFile> f)
{
    const std::function<sequence_meta_infos_or_error()> readFunc = [this, f]
    {
        return AllLoader::loadSequenceMetaInfosFromFile(mpc, f.get());
    };

    return performIoOrOpenErrorPopup(readFunc);
}

sequence_or_error
AbstractDisk::loadOneSequenceFromAllFile(std::shared_ptr<MpcFile> f,
                                         SequenceIndex sourceIndexInAllFile,
                                         SequenceIndex destIndexInMpcMemory)
{
    const std::function<sequence_or_error()> readFunc =
        [this, f, sourceIndexInAllFile, destIndexInMpcMemory]
    {
        auto result = AllLoader::loadOneSequenceFromFile(
            mpc, f.get(), sourceIndexInAllFile, destIndexInMpcMemory);
        return result;
    };

    return performIoOrOpenErrorPopup(readFunc);
}

template <typename return_type>
void AbstractDisk::performIoOrOpenErrorPopupNonReturning(
    std::function<tl::expected<return_type, mpc_io_error_msg>()> ioFunc)
{
    (void)performIoOrOpenErrorPopup(ioFunc);
}

template <typename return_type>
tl::expected<return_type, mpc_io_error_msg>
AbstractDisk::performIoOrOpenErrorPopup(
    std::function<tl::expected<return_type, mpc_io_error_msg>()> ioFunc)
{
    auto showPopup = [this](const std::string &msg)
    {
        auto ls = mpc.getLayeredScreen();
        ls->postToUiThread(utils::Task(
            [ls, msg]
            {
                ls->showPopupAndThenReturnToLayer(msg, 1000, 0);
            }));
    };

    try
    {
        tl::expected<return_type, mpc_io_error_msg> ioFuncRes = ioFunc();

        if (!ioFuncRes.has_value())
        {
            showPopup(ioFuncRes.error());
        }

        return ioFuncRes;
    }
    catch (const std::exception &e)
    {
        MLOG("I/O error: " + std::string(e.what()));
        auto msg = mpc_io_error_msg{"I/O error! See logs for info"};
        showPopup(msg);
        return tl::make_unexpected(msg);
    }
}
