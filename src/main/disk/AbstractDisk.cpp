#include "AbstractDisk.hpp"

#include <Mpc.hpp>
#include <Util.hpp>

#include <disk/MpcFile.hpp>
#include <disk/ApsLoader.hpp>
#include <disk/ProgramLoader.hpp>

#include <file/wav/WavFile.hpp>
#include <file/mid/MidiWriter.hpp>
#include <file/mid/MidiReader.hpp>
#include <file/pgmwriter/PgmWriter.hpp>
#include <file/sndwriter/SndWriter.hpp>
#include <file/sndreader/SndReader.hpp>
#include <file/aps/ApsParser.hpp>
#include <file/all/AllParser.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sound.hpp>

#include <sequencer/Track.hpp>

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/LoadAProgramScreen.hpp>
#include <lcdgui/screens/window/SaveAProgramScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <StrUtil.hpp>
#include <stdexcept>

using namespace mpc::disk;

using namespace mpc::nvram;

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

AbstractDisk::AbstractDisk(mpc::Mpc& _mpc)
        : mpc (_mpc)
{
}

AbstractDisk::~AbstractDisk()
{
    while (!programSoundsSaveThread.joinable())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    programSoundsSaveThread.join();
}

std::shared_ptr<MpcFile> AbstractDisk::getFile(int i)
{
    if (i >= files.size())
    {
        return {};
    }

    return files[i];
}

std::vector<std::string> AbstractDisk::getFileNames()
{
    std::vector<std::string> res;
    transform(files.begin(), files.end(), back_inserter(res), [](std::shared_ptr<MpcFile> f) { return f->getName(); });
    return res;
}

std::string AbstractDisk::getFileName(int i)
{
    return files[i]->getName();
}

std::vector<std::string> AbstractDisk::getParentFileNames()
{
    std::vector<std::string> res;

    for (auto& f : parentFiles)
    {
        res.push_back(f->getName().length() < 8 ? f->getName() : f->getName().substr(0, 8));
    }

    return res;
}

bool AbstractDisk::deleteSelectedFile()
{
    auto loadScreen = mpc.screens->get<LoadScreen>();
    return files[loadScreen->fileLoad]->del();
}

std::vector<std::shared_ptr<MpcFile>>& AbstractDisk::getAllFiles() {
    return allFiles;
}

std::shared_ptr<MpcFile> AbstractDisk::getParentFile(int i)
{
    return parentFiles[i];
}

void AbstractDisk::writeSnd(std::shared_ptr<Sound> s, std::string fileName)
{
    std::function<file_or_error()> writeFunc = [&] {
        auto name = mpc::Util::getFileName(fileName == "" ? s->getName() + ".SND" : fileName);
        auto f = newFile(name);
        auto sw = SndWriter(s.get());
        auto& sndArray = sw.getSndFileArray();
        f->setFileData(sndArray);
        flush();
        initFiles();
        return f;
    };

    performIoOrOpenErrorPopup(writeFunc);
}

void AbstractDisk::writeWav(std::shared_ptr<Sound> s, std::string fileName)
{
    std::function<file_or_error()> writeFunc = [&] {
        auto name = mpc::Util::getFileName(fileName == "" ? s->getName() + ".WAV" : fileName);
        auto f = newFile(name);
        auto outputStream = f->getOutputStream();
        auto isMono = s->isMono();
        auto data = s->getSampleData();
        auto wavFile = WavFile::writeWavStream(
                outputStream,
                isMono ? 1 : 2, data->size() / (isMono ? 1 : 2),
                16,
                s->getSampleRate());

        if (isMono)
        {
            wavFile.writeFrames(*data, data->size());
        }
        else
        {
            std::vector<float> interleaved;

            for (int i = 0; i < (int) (data->size() * 0.5); i++)
            {
                interleaved.push_back((*data)[i]);
                interleaved.push_back((*data)[(int) (i + data->size() * 0.5)]);
            }

            wavFile.writeFrames(interleaved, data->size() * 0.5);
        }

        wavFile.close();
        flush();
        initFiles();

        return f;
    };

    performIoOrOpenErrorPopup(writeFunc);
}

void AbstractDisk::writeMid(std::shared_ptr<mpc::sequencer::Sequence> s, std::string fileName)
{
    std::function<file_or_error()> writeFunc = [&] {
        auto f = newFile(fileName);
        MidiWriter writer(s.get());
        writer.writeToOStream(f->getOutputStream());
        flush();
        initFiles();
        auto popupScreen = mpc.screens->get<PopupScreen>();
        popupScreen->setText("Saving " + fileName);
        popupScreen->returnToScreenAfterMilliSeconds("save", 400);
        mpc.getLayeredScreen()->openScreen("popup");
        return f;
    };

    performIoOrOpenErrorPopup(writeFunc);
}

bool AbstractDisk::checkExists(std::string fileName)
{
    initFiles();

    const auto path = fs::path(fileName);

    return std::any_of(allFiles.begin(), allFiles.end(), [path](const std::shared_ptr<MpcFile>& file){
        const auto path2 = fs::path(file->getName());
        const auto nameIsSame = StrUtil::eqIgnoreCase(path2.stem().string(), path.stem().string());
        const auto extIsSame = StrUtil::eqIgnoreCase(path2.extension().string(), path.extension().string());
        return nameIsSame && extIsSame;
    });
}

std::shared_ptr<MpcFile> AbstractDisk::getFile(const std::string& fileName)
{
    auto tempfileName = StrUtil::replaceAll(fileName, ' ', "");

    for (auto& f : files)
    {
        if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), tempfileName))
            return f;
    }

    for (auto& f : allFiles)
    {
        if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), tempfileName))
            return f;
    }

    return {};
}

bool AbstractDisk::isRoot()
{
    return getPathDepth() == 0;
}

bool AbstractDisk::deleteRecursive(std::weak_ptr<MpcFile> _toDelete)
{
    auto toDelete = _toDelete.lock();

    if (toDelete->isDirectory())
    {
        for (auto& f : toDelete->listFiles())
        {
            if (f->getName() == "" || f->getName() == "." || f->getName() == "..")
                continue;

            deleteRecursive(f);
        }
    }

    return toDelete->del();
}


void AbstractDisk::writePgm(std::shared_ptr<Program> p, const std::string& fileName)
{
    std::function<file_or_error()> writeFunc = [&] {
        auto f = newFile(fileName);
        PgmWriter writer(p.get(), mpc.getSampler());
        auto bytes = writer.get();
        f->setFileData(bytes);

        auto popupScreen = mpc.screens->get<PopupScreen>();
        popupScreen->setText("Saving " + fileName);
        mpc.getLayeredScreen()->openScreen("popup");

        auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>();

        if (saveAProgramScreen->save != 0)
        {
            std::vector<std::shared_ptr<Sound>> sounds;

            for (auto& n : p->getNotesParameters())
            {
                const auto soundIndex = n->getSoundIndex();

                if (soundIndex != -1)
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

                auto isWav = saveAProgramScreen->save == 2;

                programSoundsSaveThread = std::thread([this, isWav, sounds]{
                    std::this_thread::sleep_for(std::chrono::milliseconds(700));
                    soundSaver = std::make_unique<SoundSaver>(mpc, sounds, isWav);
                });
            }
            else
            {
                popupScreen->returnToScreenAfterMilliSeconds("save", 700);
            }
        }
        else
        {
            popupScreen->returnToScreenAfterMilliSeconds("save", 700);
        }

        flush();
        initFiles();
        return f;
    };

    performIoOrOpenErrorPopup(writeFunc);
}

void AbstractDisk::writeAps(const std::string& fileName)
{
    std::function<file_or_error()> writeFunc = [&] {
        auto f = newFile(fileName);
        auto apsName = f->getNameWithoutExtension();
        ApsParser apsParser(mpc, apsName);
        auto bytes = apsParser.getBytes();
        f->setFileData(bytes);

        auto popupScreen = mpc.screens->get<PopupScreen>();
        popupScreen->setText("Saving " + fileName);
        mpc.getLayeredScreen()->openScreen("popup");

        auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>();

        if (saveAProgramScreen->save != 0 && mpc.getSampler()->getSoundCount() > 0)
        {
            if (programSoundsSaveThread.joinable())
            {
                programSoundsSaveThread.join();
            }

            programSoundsSaveThread = std::thread([this, saveAProgramScreen]{
                std::this_thread::sleep_for(std::chrono::milliseconds(700));
                soundSaver = std::make_unique<SoundSaver>(mpc, mpc.getSampler()->getSounds(), saveAProgramScreen->save == 2);
            });
        }
        else
        {
            popupScreen->returnToScreenAfterMilliSeconds("save", 700);
        }

        flush();
        initFiles();

        return f;
    };

    performIoOrOpenErrorPopup(writeFunc);
}

void AbstractDisk::writeAll(const std::string& fileName)
{
    std::function<file_or_error()> writeFunc = [&] {
        auto f = newFile(fileName);
        AllParser allParser(mpc);
        auto bytes = allParser.getBytes();
        f->setFileData(bytes);

        flush();
        initFiles();

        auto popupScreen = mpc.screens->get<PopupScreen>();
        popupScreen->setText("         Saving ...");
        popupScreen->returnToScreenAfterMilliSeconds("save", 400);
        mpc.getLayeredScreen()->openScreen("popup");

        return f;
    };

    performIoOrOpenErrorPopup(writeFunc);
}

void AbstractDisk::writeMidiControlPreset(std::shared_ptr<MidiControlPreset> preset)
{
    std::function<preset_or_error()> ioFunc = [preset, this] {
        std::vector<char> data;

        const uint8_t fileFormatVersion = 2;

        data.push_back(static_cast<char>(fileFormatVersion));

        data.push_back(preset->autoloadMode);

        for (char i : preset->name)
        {
            data.push_back(i);
        }

        for (int i = preset->name.length(); i < 16; i++)
        {
            data.push_back(' ');
        }

        for (auto& r : preset->rows)
        {
            const auto rowData = r.toBytes();

            for (auto& b : rowData)
            {
                data.push_back(b);
            }
        }

        const auto presetPath = mpc.paths->midiControlPresetsPath() / (preset->name + ".vmp");

        set_file_data(presetPath, data);

        return preset;
    };

    performIoOrOpenErrorPopup(ioFunc);
}

void readMidiControlPresetV1(const std::vector<char> &data, const std::shared_ptr<MidiControlPreset>& preset)
{
    preset->rows.clear();
    preset->name = "";

    preset->autoloadMode = data[0];

    std::string presetName;

    for (int i = 0; i < 16; i++)
    {
        if (data[i + 1] == ' ') continue;
        presetName.push_back(data[i + 1]);
    }

    preset->name = presetName;

    int pointer = 17;

    while (pointer < data.size())
    {
        std::string name;
        char c;

        while ((c = data[pointer++]) != ' ' && pointer < data.size())
        {
            name.push_back(c);
        }
        
        static constexpr char extraTag[] = "(extra)";
        constexpr size_t extraLen = sizeof(extraTag) - 1;

        if (pointer + extraLen <= data.size() &&
            std::equal(extraTag, extraTag + extraLen, data.begin() + pointer))
        {
            name.push_back(' ');
            name.append(extraTag);
            pointer += extraLen + 1;
        }
        
        const bool isNote = data[pointer++] == 1;
        const char channel = data[pointer++];
        const char number = data[pointer++];

        if (isNote)
        {
            const auto cmd = MidiControlCommand(
                    name,
                    MidiControlCommand::MidiMessageType::NOTE,
                    channel,
                    number);
            preset->rows.emplace_back(cmd);
            continue;
        }

        const auto cmd = MidiControlCommand(
                name,
                MidiControlCommand::MidiMessageType::CC,
                channel,
                number,
                -1);
        preset->rows.emplace_back(cmd);
    }
}

void readMidiControlPresetV2(const std::vector<char> &data, const std::shared_ptr<MidiControlPreset> &preset)
{
    if (data[0] != 2)
    {
        throw std::runtime_error("File data is not actually a version 2 MIDI control preset");
    }

    preset->rows.clear();
    preset->name = "";

    preset->autoloadMode = data[1];

    std::string presetName;

    for (int i = 0; i < 16; i++)
    {
        if (data[i + 2] == ' ') continue;
        presetName.push_back(data[i + 2]);
    }

    preset->name = presetName;

    size_t pointer = 18;
    size_t currentChunkSize;

    while (pointer < data.size())
    {
        currentChunkSize = 0;

        while(pointer < data.size())
        {
            currentChunkSize++;

            if (data[pointer++] == ' ')
            {
                break;
            }
        }

        pointer += 4;
        currentChunkSize += 4;

        const auto chunk = std::vector<char>(data.begin() + (pointer - currentChunkSize), data.begin() + pointer);
        preset->rows.emplace_back(MidiControlCommand::fromBytes(chunk));
    }
}

void AbstractDisk::readMidiControlPreset(const fs::path& p, const std::shared_ptr<MidiControlPreset> &preset)
{
    MLOG("Trying to read MIDI control preset at path " + p.string());

    std::function<preset_or_error()> ioFunc = [p, preset] () -> preset_or_error {

        auto data = get_file_data(p);

        if (data.size() < 681)
        {
            return tl::make_unexpected(mpc_io_error_msg{"MIDI control preset file is smaller than 681 bytes: " + p.string()});
        }
        
        if (data.size() == 681 || data.size() == 847)
        {
            readMidiControlPresetV1(data, preset);
        }
        else
        {
            readMidiControlPresetV2(data, preset);
        }

        return preset;
    };

    performIoOrOpenErrorPopup(ioFunc);
}

wav_or_error AbstractDisk::readWavMeta(std::shared_ptr<MpcFile> f)
{
    std::function<wav_or_error()> readFunc = [f] {
        return WavFile::readWavStream(f->getInputStream());
    };

    return performIoOrOpenErrorPopup(readFunc);
}

sound_or_error AbstractDisk::readWav2(
        std::shared_ptr<MpcFile> f,
        std::function<sound_or_error(std::shared_ptr<WavFile>)> onSuccess)
{
    std::function<sound_or_error()> readFunc = [f, onSuccess] { return WavFile::readWavStream(f->getInputStream()).and_then(onSuccess); };
    return performIoOrOpenErrorPopup(readFunc);
}

sound_or_error AbstractDisk::readSnd2(std::shared_ptr<MpcFile> f,
                                      std::function<sound_or_error(std::shared_ptr<SndReader>)> onSuccess)
{
    std::function<sound_or_error()> readFunc = [f, onSuccess] { return onSuccess(std::make_shared<SndReader>(f.get())); };
    return performIoOrOpenErrorPopup(readFunc);
}

sequence_or_error AbstractDisk::readMid2(std::shared_ptr<MpcFile> f)
{
    std::function<sequence_or_error()> readFunc = [this, f]() -> sequence_or_error {
        if (!f)
        {
            return tl::make_unexpected(mpc_io_error_msg{"Empty file"});
        }

        auto fStream = f->getInputStream();
        auto newSeq = mpc.getSequencer()->createSeqInPlaceHolder();

        newSeq->init(0);

        MidiReader midiReader(fStream, newSeq);
        midiReader.parseSequence(mpc);
        
        if (newSeq->getName().empty())
        {
            newSeq->setName(f->getNameWithoutExtension());
        }

        return newSeq;
    };

    return performIoOrOpenErrorPopup(readFunc);
}

void AbstractDisk::readPgm2(std::shared_ptr<MpcFile> f, std::shared_ptr<Program> p)
{
    new std::thread([this, f, p]() {
        std::function<tl::expected<bool, mpc_io_error_msg>()> readFunc = [this, f, p]{
            ProgramLoader::loadProgram(mpc, f, p);
            return true;
        };

        performIoOrOpenErrorPopup(readFunc);
    });
}

void AbstractDisk::readAps2(std::shared_ptr<MpcFile> f, std::function<void()> onSuccess)
{
    new std::thread([this, f, onSuccess]() {
        std::function<tl::expected<bool, mpc_io_error_msg>()> readFunc = [&] {
            ApsLoader::load(mpc, f);
            onSuccess();
            return true;
        };

        performIoOrOpenErrorPopup(readFunc);
    });
}

void AbstractDisk::readAll2(std::shared_ptr<MpcFile> f, std::function<void()> onSuccess)
{
    std::function<tl::expected<bool, mpc_io_error_msg>()> readFunc = [this, f, onSuccess]{
        AllLoader::loadEverythingFromFile(mpc, f.get());
        onSuccess();
        return true;
    };

    performIoOrOpenErrorPopup(readFunc);
}

sequences_or_error AbstractDisk::readSequencesFromAll2(std::shared_ptr<MpcFile> f)
{
    std::function<sequences_or_error()> readFunc = [this, f]{
        auto result = AllLoader::loadOnlySequencesFromFile(mpc, f.get());
        auto loadScreen = mpc.screens->get<LoadScreen>();
        loadScreen->fileLoad = 0;
        return result;
    };

    return performIoOrOpenErrorPopup(readFunc);
}

template<typename return_type>
tl::expected<return_type, mpc_io_error_msg> AbstractDisk::performIoOrOpenErrorPopup(std::function<tl::expected<return_type, mpc_io_error_msg>()> ioFunc)
{
    try {
        tl::expected<return_type, mpc_io_error_msg> ioFuncRes = ioFunc();

        if (!ioFuncRes.has_value())
        {
            showPopup(ioFuncRes.error());
        }

        return ioFuncRes;
    } catch (const std::exception& e) {
        MLOG("I/O error: " + std::string(e.what()));
        auto msg = mpc_io_error_msg{"I/O error! See logs for info"};
        showPopup(msg);
        return tl::make_unexpected(msg);
    }
}

void AbstractDisk::showPopup(mpc_io_error_msg& msg)
{
    auto popupScreen = mpc.screens->get<PopupScreen>();
    popupScreen->setText(msg);
    auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

    if (currentScreenName == "load-a-sequence")
    {
        currentScreenName = "load";
    }

    popupScreen->returnToScreenAfterMilliSeconds(currentScreenName, 1000);
    mpc.getLayeredScreen()->openScreen("popup");
}
