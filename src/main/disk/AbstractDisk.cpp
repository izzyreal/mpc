#include "AbstractDisk.hpp"

#include <Mpc.hpp>
#include <Util.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

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

#include <file/FileUtil.hpp>
#include <lang/StrUtil.hpp>

using namespace mpc::disk;
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

using namespace moduru::lang;
using namespace moduru::file;

AbstractDisk::AbstractDisk(mpc::Mpc& _mpc)
        : mpc (_mpc)
{
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
    auto loadScreen = mpc.screens->get<LoadScreen>("load");
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
        auto sndArray = sw.getSndFileArray();
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
        return f;
    };

    performIoOrOpenErrorPopup(writeFunc);
}

bool AbstractDisk::checkExists(std::string fileName)
{
    initFiles();

    auto fileNameSplit = FileUtil::splitName(fileName);

    for (auto& file : getAllFiles())
    {
        auto name = FileUtil::splitName(file->getName());
        auto nameIsSame = StrUtil::eqIgnoreCase(name[0], fileNameSplit[0]);
        auto extIsSame = StrUtil::eqIgnoreCase(name[1], fileNameSplit[1]);

        if (nameIsSame && extIsSame)
            return true;
    }

    return false;
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

        auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");

        if (saveAProgramScreen->save != 0)
        {
            std::vector<std::shared_ptr<Sound>> sounds;

            for (auto& n : p->getNotesParameters())
            {
                if (n->getSoundIndex() != -1)
                    sounds.push_back(mpc.getSampler()->getSound(n->getSoundIndex()));
            }

            auto isWav = saveAProgramScreen->save == 2;
            soundSaver = std::make_unique<SoundSaver>(mpc, sounds, isWav);
        }
        else
        {
            mpc.getLayeredScreen()->openScreen("save");
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

        auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");

        if (saveAProgramScreen->save != 0)
        {
            soundSaver = std::make_unique<SoundSaver>(mpc, mpc.getSampler()->getSounds(), saveAProgramScreen->save == 2);
        }
        else
        {
            mpc.getLayeredScreen()->openScreen("save");
        }

        flush();
        initFiles();

        auto popupScreen = mpc.screens->get<PopupScreen>("popup");
        popupScreen->setText("Saving " + fileName);
        popupScreen->returnToScreenAfterMilliSeconds("save", 400);
        mpc.getLayeredScreen()->openScreen("popup");

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

        auto popupScreen = mpc.screens->get<PopupScreen>("popup");
        popupScreen->setText("         Saving ...");
        popupScreen->returnToScreenAfterMilliSeconds("save", 400);
        mpc.getLayeredScreen()->openScreen("popup");

        return f;
    };

    performIoOrOpenErrorPopup(writeFunc);
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
        auto loadScreen = mpc.screens->get<LoadScreen>("load");
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
    auto popupScreen = mpc.screens->get<PopupScreen>("popup");
    popupScreen->setText(msg);
    auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
    popupScreen->returnToScreenAfterMilliSeconds(currentScreenName, 1000);
    mpc.getLayeredScreen()->openScreen("popup");
}
