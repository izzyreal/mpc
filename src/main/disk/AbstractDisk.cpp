#include "AbstractDisk.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <disk/MpcFile.hpp>
#include <file/wav/WavFile.hpp>
#include <file/mid/MidiWriter.hpp>
#include <file/mid/MidiReader.hpp>
#include <file/pgmwriter/PgmWriter.hpp>
#include <file/sndwriter/SndWriter.hpp>
#include <file/sndreader/SndReader.hpp>
#include <disk/ApsLoader.hpp>
#include <disk/ProgramLoader.hpp>
#include <file/aps/ApsParser.hpp>
#include <file/all/AllParser.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <sequencer/Track.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/LoadAProgramScreen.hpp>
#include <lcdgui/screens/window/SaveAProgramScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

#include <file/FileUtil.hpp>
#include <lang/StrUtil.hpp>

#include <cmath>

using namespace mpc::disk;
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
: mpc (_mpc),
errorFunc ([&](mpc_io_error e){
    MLOG(e.log_msg);
    new std::thread([&](){
        auto popupScreen = mpc.screens->get<PopupScreen>("popup");
        popupScreen->setText("Unknown disk error!");
        auto currentScreenName = mpc.getLayeredScreen().lock()->getCurrentScreenName();
        popupScreen->returnToScreenAfterMilliSeconds(currentScreenName, 1000);
        mpc.getLayeredScreen().lock()->openScreen("popup");
    });
})
{
}

std::shared_ptr<MpcFile> AbstractDisk::getFile(int i)
{
    if (i >= files.size()) return {};
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
        res.push_back(f->getName().length() < 8 ? f->getName() : f->getName().substr(0, 8));
    
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
    auto writeSndFunc = [&](std::shared_ptr<MpcFile> f){ return writeSnd2(s, f); };
    auto name = mpc::Util::getFileName(fileName == "" ? s->getName() + ".SND" : fileName);
    newFile2(name)
    .and_then(writeSndFunc)
    .map_error(errorFunc);
}

file_or_error AbstractDisk::writeSnd2(std::shared_ptr<Sound> s, std::shared_ptr<MpcFile> f)
{
    std::string msg;
    
    try {
        auto sw = SndWriter(s.get());
        auto sndArray = sw.getSndFileArray();
        f->setFileData(sndArray);
        flush();
        initFiles();
        return f;
    } catch (const std::exception& e) {
        msg = e.what();
    }
    
    return tl::make_unexpected(mpc_io_error{"Could not write SND file due to: " + msg});
}

void AbstractDisk::writeWav(std::shared_ptr<Sound> s, std::string fileName)
{
    auto writeWavFunc = [&](std::shared_ptr<MpcFile> f){ return writeWav2(s, f); };
    
    auto name = mpc::Util::getFileName(fileName == "" ? s->getName() + ".WAV" : fileName);
    newFile2(name)
    .and_then(writeWavFunc)
    .map_error(errorFunc);
}

file_or_error AbstractDisk::writeWav2(std::shared_ptr<Sound> sound, std::shared_ptr<MpcFile> f)
{
    auto outputStream = f->getOutputStream();
    auto isMono = sound->isMono();
    auto data = sound->getSampleData();
    
    std::string msg;
    
    try {
        auto wavFile = WavFile::writeWavStream(outputStream, isMono ? 1 : 2, data->size() / (isMono ? 1 : 2), 16, sound->getSampleRate());
        
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
        
    } catch (const std::exception& e) {
        msg = e.what();
    }
    
    return tl::make_unexpected(mpc_io_error{"Could not write WAV file due to: " + msg});
}

void AbstractDisk::writeMid(std::shared_ptr<mpc::sequencer::Sequence> s, std::string fileName)
{
    auto writeMidFunc = [&](std::shared_ptr<MpcFile> f){ return writeMid2(s, f); };
    newFile2(fileName)
    .and_then(writeMidFunc)
    .map_error(errorFunc);
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
    auto writePgmFunc = [&](std::shared_ptr<MpcFile> f){ return writePgm2(p, f); };

    newFile2(fileName)
    .and_then(writePgmFunc)
    .map_error(errorFunc);
}

void AbstractDisk::writeAps(const std::string& fileName)
{
    auto writeApsFunc = [&](std::shared_ptr<MpcFile> f){ return writeAps2(f); };
    
    auto onSuccess = [&](std::shared_ptr<MpcFile>) {
        auto popupScreen = mpc.screens->get<PopupScreen>("popup");
        popupScreen->setText("Saving " + StrUtil::padRight(fileName, " ", 16) + ".APS");
        popupScreen->returnToScreenAfterMilliSeconds("save", 400);
        mpc.getLayeredScreen().lock()->openScreen("popup");
    };
    
    newFile2(fileName)
    .and_then(writeApsFunc)
    .map(onSuccess)
    .map_error(errorFunc);
}

void AbstractDisk::writeAll(const std::string& fileName)
{
    auto writeAllFunc = [&](std::shared_ptr<MpcFile> f){ return writeAll2(f); };
    
    auto onSuccess = [&](std::shared_ptr<MpcFile>) {
        auto popupScreen = mpc.screens->get<PopupScreen>("popup");
        popupScreen->setText("         Saving ...");
        popupScreen->returnToScreenAfterMilliSeconds("save", 400);
        mpc.getLayeredScreen().lock()->openScreen("popup");
    };
    
    newFile2(fileName)
    .and_then(writeAllFunc)
    .map(onSuccess)
    .map_error(errorFunc);
}

file_or_error AbstractDisk::newFile2(const std::string& name)
{
    std::string msg;
    
    try { return newFile(name); }
    catch (const std::exception& e) { msg = e.what(); }
    
    return tl::make_unexpected(mpc_io_error{"Could not create new file: " + msg});
}

file_or_error AbstractDisk::writeMid2(std::shared_ptr<mpc::sequencer::Sequence> s, std::shared_ptr<MpcFile> f)
{
    std::string msg;
    
    try {
        MidiWriter writer(s.get());
        writer.writeToOStream(f->getOutputStream());
        flush();
        initFiles();
        return f;
    } catch (const std::exception& e) { msg = e.what(); }
    
    return tl::make_unexpected(mpc_io_error{"Could not write MID file: " + msg});
}

file_or_error AbstractDisk::writePgm2(std::shared_ptr<mpc::sampler::Program> p, std::shared_ptr<MpcFile> f)
{
    std::string msg;
    
    try {
        PgmWriter writer(p.get(), mpc.getSampler());
        auto bytes = writer.get();
        f->setFileData(bytes);
        
        auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");
        
        if (saveAProgramScreen->save != 0)
        {
            std::vector<std::weak_ptr<Sound>> sounds;
            
            for (auto& n : p->getNotesParameters())
            {
                if (n->getSoundIndex() != -1)
                    sounds.push_back(mpc.getSampler().lock()->getSound(n->getSoundIndex()).lock());
            }
            
            auto isWav = saveAProgramScreen->save == 2;
            soundSaver = std::make_unique<SoundSaver>(mpc, sounds, isWav);
        }
        else
        {
            mpc.getLayeredScreen().lock()->openScreen("save");
        }
        
        flush();
        initFiles();
        return f;
    } catch (const std::exception& e) { msg = e.what(); }
    
    return tl::make_unexpected(mpc_io_error{"Could not write PGM file: " + msg});
}

file_or_error AbstractDisk::writeAps2(std::shared_ptr<MpcFile> f)
{
    std::string msg;
    
    try {
        auto apsName = f->getNameWithoutExtension();
        ApsParser apsParser(mpc, apsName);
        auto bytes = apsParser.getBytes();
        f->setFileData(bytes);

        auto saveAProgramScreen = mpc.screens->get<SaveAProgramScreen>("save-a-program");
        
        if (saveAProgramScreen->save != 0)
        {
            soundSaver = std::make_unique<SoundSaver>(mpc, mpc.getSampler().lock()->getSounds(), saveAProgramScreen->save == 2);
        }
        else
        {
            mpc.getLayeredScreen().lock()->openScreen("save");
        }

        flush();
        initFiles();
        return f;
    } catch (const std::exception& e) { msg = e.what(); }
    
    return tl::make_unexpected(mpc_io_error{"Could not write APS file: " + msg});
}

file_or_error AbstractDisk::writeAll2(std::shared_ptr<MpcFile> f)
{
    std::string msg;
    
    try {
        AllParser allParser(mpc, f->getNameWithoutExtension());
        auto bytes = allParser.getBytes();
        f->setFileData(bytes);
        
        flush();
        initFiles();
        return f;
    } catch (const std::exception& e) { msg = e.what(); }
    
    return tl::make_unexpected(mpc_io_error{"Could not write ALL file: " + msg});
}

wav_or_error AbstractDisk::readWavMeta(std::shared_ptr<MpcFile> f)
{
    std::string msg;
    
    try {
        auto inputStream = f->getInputStream();
        auto wavFile = WavFile::readWavStream(inputStream);
        return wavFile;
    }
    catch (const std::exception& e)
    {
        msg = e.what();
    }
        
    return tl::make_unexpected(mpc_io_error{"Could not read WAV file: " + msg});
}

sound_or_error AbstractDisk::readWav2(std::shared_ptr<MpcFile> f, bool shouldBeConverted)
{
    std::string msg;
    
    try {
        auto sound = mpc.getSampler().lock()->addSound().lock();
        auto inputStream = f->getInputStream();
        auto wavFile = WavFile::readWavStream(inputStream);
        
        if (wavFile.getValidBits() != 16 && !shouldBeConverted)
        {
            wavFile.close();
            return tl::make_unexpected(mpc_io_error{ f->getName() + " is not a 16 bit .WAV file. " + std::to_string(wavFile.getValidBits()) + " .WAV files are not supported." });
        }

        if ( (wavFile.getSampleRate() < 8000 || wavFile.getSampleRate() > 44100) && !shouldBeConverted)
        {
            wavFile.close();
            return tl::make_unexpected(mpc_io_error{ f->getName() + " has a sample rate of " + std::to_string(wavFile.getSampleRate()) + ". Sample rate has to be between 8000 and 44100." });
        }
        
        sound->setName(f->getNameWithoutExtension());
        
        int sampleRate = wavFile.getSampleRate();
        
        if (sampleRate > 44100 && shouldBeConverted) sampleRate = 44100;
        
        sound->setSampleRate(sampleRate);
        
        sound->setLevel(100);
        
        int numChannels = wavFile.getNumChannels();
        
        auto sampleData = sound->getSampleData();
        
        if (numChannels == 1)
        {
            wavFile.readFrames(*sampleData, wavFile.getNumFrames());
        }
        else
        {
            std::vector<float> interleaved;
            wavFile.readFrames(interleaved, wavFile.getNumFrames());
            
            for (int i = 0; i < interleaved.size(); i += 2)
                sampleData->push_back(interleaved[i]);
            
            for (int i = 1; i < interleaved.size(); i += 2)
                sampleData->push_back(interleaved[i]);
        }
        
        if (wavFile.getSampleRate() > 44100 && shouldBeConverted)
        {
            auto tempSound = std::make_shared<mpc::sampler::Sound>();
            mpc::sampler::Sampler::resample(*sampleData, wavFile.getSampleRate(), tempSound);
            auto tempData = *tempSound->getSampleData();
            sampleData->swap(tempData);
        }
        
        sound->setMono(numChannels == 1);
        
        if (wavFile.getNumSampleLoops() > 0)
        {
            auto& sampleLoop = wavFile.getSampleLoop();
            sound->setLoopTo(sampleLoop.start);
            auto currentEnd = sound->getEnd();
            sound->setEnd(sampleLoop.end <= 0 ? currentEnd : sampleLoop.end);
            sound->setLoopEnabled(true);
        }
        
        const auto tuneFactor = (float)(sound->getSampleRate() / 44100.0);
        const auto rateToTuneBase = (float)(pow(2, (1.0 / 12.0)));
        
        int tune = (int)(floor(log(tuneFactor) / log(rateToTuneBase) * 10.0));
        
        if (tune < -120)
            tune = -120;
        else if (tune > 120)
            tune = 120;
        
        sound->setTune(tune);
        return sound;
    }
    catch (const std::exception& e)
    {
        msg = e.what();
    }
        
    return tl::make_unexpected(mpc_io_error{"Could not read WAV file: " + msg});
}

sound_or_error AbstractDisk::readSnd2(std::shared_ptr<MpcFile> f)
{
    std::string msg;
    
    try {
        SndReader sndReader(f.get());
        
        if (sndReader.isHeaderValid())
        {
            auto sound = mpc.getSampler().lock()->addSound().lock();
            
            sndReader.readData(*sound->getSampleData());
            sound->setMono(sndReader.isMono());
            sound->setStart(sndReader.getStart());
            sound->setEnd(sndReader.getEnd());
            sound->setLoopTo(sndReader.getEnd() - sndReader.getLoopLength());
            sound->setSampleRate(sndReader.getSampleRate());
            sound->setName(sndReader.getName());
            sound->setLoopEnabled(sndReader.isLoopEnabled());
            sound->setLevel(sndReader.getLevel());
            sound->setTune(sndReader.getTune());
            sound->setBeatCount(sndReader.getNumberOfBeats());
            
            return sound;
        }
        else
        {
            msg = "Invalid SND header";
        }
    } catch (const std::exception& e) { msg = e.what(); };

    return tl::make_unexpected(mpc_io_error{"Could not read SND file: " + msg});
}

sequence_or_error AbstractDisk::readMid2(std::shared_ptr<MpcFile> f)
{
    if (!f) return tl::make_unexpected(mpc_io_error{"Empty MpcFile passed to AbstractDisk::readMid2"});
    
    std::string msg;
    
    try {
        auto newSeq = mpc.getSequencer().lock()->createSeqInPlaceHolder().lock();
        newSeq->init(2);
        MidiReader midiReader(f->getInputStream(), newSeq);
        midiReader.parseSequence(mpc);
        return newSeq;
    } catch (const std::exception& e) { msg = e.what(); };
    
    return tl::make_unexpected(mpc_io_error{"Could not read MID file: " + msg});
}

void AbstractDisk::readPgm2(std::shared_ptr<MpcFile> f)
{
    new std::thread([&, f]() {
        std::string msg;
        
        try {
            auto loadScreen = mpc.screens->get<LoadScreen>("load");
            auto loadAProgramScreen = mpc.screens->get<LoadAProgramScreen>("load-a-program");
            auto bus = mpc.getSequencer().lock()->getActiveTrack().lock()->getBus();
            
            auto activePgm = bus == 0 ? 0 : mpc.getDrum(bus)->getProgram();
            ProgramLoader::loadProgram(mpc, f, loadAProgramScreen->loadReplaceSound ? activePgm : -1);
        } catch (const std::exception& e) {
            std::string msg = e.what();
            errorFunc(mpc_io_error{"Could not read PGM file: " + msg});
        };
    });
}

void AbstractDisk::readAps2(std::shared_ptr<MpcFile> f, std::function<void()> on_success)
{
    new std::thread([&, f, on_success]() {
        try {
            ApsLoader::load(mpc, f);
            on_success();
        } catch (const std::exception& e) {
            std::string msg = e.what();
            errorFunc(mpc_io_error{"Could not read APS file: " + msg});
        };
    });
}

void AbstractDisk::readAll2(std::shared_ptr<MpcFile> f, std::function<void()> on_success)
{
    try {
        AllLoader::loadEverythingFromFile(mpc, f.get());
        on_success();
    } catch (const std::exception& e) {
        std::string msg = e.what();
        errorFunc(mpc_io_error{"Could not read ALL file: " + msg});
    };
}

sequences_or_error AbstractDisk::readSequencesFromAll2(std::shared_ptr<MpcFile> f)
{
    std::string msg;
    
    try
    {
        auto result = AllLoader::loadOnlySequencesFromFile(mpc, f.get());
        auto loadScreen = mpc.screens->get<LoadScreen>("load");
        loadScreen->fileLoad = 0;
        return result;
    } catch (const std::exception& e) {
        msg = e.what();
        errorFunc(mpc_io_error{"Could not read ALL file: " + msg});
    };
    
    return tl::make_unexpected(mpc_io_error{"Could not read ALL file: " + msg});
}
