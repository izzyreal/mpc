#include <disk/SoundLoader.hpp>

#include <Mpc.hpp>
#include <disk/MpcFile.hpp>
#include <file/sndreader/SndReader.hpp>
#include <file/wav/WavFile.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lang/StrUtil.hpp>
#include <file/File.hpp>

#include <cmath>

using namespace mpc::disk;
using namespace mpc::file::wav;
using namespace mpc::file::sndreader;
using namespace mpc::lcdgui;
using namespace moduru::lang;
using namespace std;

SoundLoader::SoundLoader(mpc::Mpc& mpc)
: mpc(mpc)
{
}
float SoundLoader::rateToTuneBase = (float)(pow(2, (1.0 / 12.0)));

SoundLoader::SoundLoader(mpc::Mpc& mpc, vector<weak_ptr<mpc::sampler::Sound>> sounds, bool replace)
: mpc(mpc)
{
    
    this->replace = replace;
    this->sounds = sounds;
}

void SoundLoader::setPartOfProgram(bool b)
{
    partOfProgram = b;
}

void SoundLoader::loadSound(shared_ptr<MpcFile> f, SoundLoaderResult& r)
{
    auto sound = mpc.getSampler().lock()->addSound().lock();
    
    string soundFileName = f->getName();
    
    auto periodIndex = soundFileName.find_last_of('.');
    string extension = "";
    string soundName = "";
    auto mono = false;
    auto sampleRate = 44100;
    auto start = 0;
    auto end = 0;
    auto loopTo = 0;
    auto level = 100;
    auto tune = 0;
    auto loopEnabled = false;
    auto beats = 4;
    
    if (periodIndex != string::npos)
    {
        extension = soundFileName.substr(periodIndex + 1, soundFileName.length());
        soundFileName = soundFileName.substr(0, periodIndex);
        soundName = soundFileName;
    }
    
    auto sampler = mpc.getSampler().lock();
    auto existingSoundIndex = sampler->checkExists(soundName);
        
    std::vector<float>& sampleData = *sound->getSampleData();
    
    if (StrUtil::eqIgnoreCase(extension, "wav"))
    {
        
        try {
            auto inputStream = f->getInputStream();
            auto wavFile = WavFile::readWavStream(inputStream);
            
            if (wavFile.getValidBits() != 16)
            {
                wavFile.close();
                r.errorMessage = "not 16bit .WAV file";
                return;
            }
            
            int numChannels = wavFile.getNumChannels();
            
            if (numChannels == 1)
            {
                wavFile.readFrames(&sampleData, wavFile.getNumFrames());
            }
            else
            {
                sampleData.clear();
                vector<float> interleaved;
                wavFile.readFrames(&interleaved, wavFile.getNumFrames());
                
                for (int i = 0; i < interleaved.size(); i += 2)
                sampleData.push_back(interleaved[i]);
                
                for (int i = 1; i < interleaved.size(); i += 2)
                sampleData.push_back(interleaved[i]);
            }
            
            size = sampleData.size();
            end = size;
            
            if (numChannels == 1)
                mono = true;
            else
                end /= 2;
            
            sampleRate = wavFile.getSampleRate();
            
            loopTo = end;
            
            if (wavFile.getNumSampleLoops() > 0)
            {
                auto& sampleLoop = wavFile.getSampleLoop();
                loopTo = sampleLoop.start;
                end = sampleLoop.end;
                loopEnabled = true;
            }
        }
        catch (const exception&)
        {
            r.errorMessage = "not 16bit .WAV file";
            return;
        }
        
        auto tuneFactor = (float)(sampleRate / 44100.0);
        
        tune = (int)(floor(logOfBase(tuneFactor, rateToTuneBase) * 10.0));
        
        if (tune < -120)
            tune = -120;
        else if (tune > 120)
            tune = 120;
    }
    else if (StrUtil::eqIgnoreCase(extension, "snd"))
    {
        SndReader sndReader(f.get());
        
        if (sndReader.isHeaderValid())
        {
            sndReader.readData(sampleData);
            size = sampleData.size();
            mono = sndReader.isMono();
            start = sndReader.getStart();
            end = sndReader.getEnd();
            loopTo = end - sndReader.getLoopLength();
            sampleRate = sndReader.getSampleRate();
            soundName = sndReader.getName();
            loopEnabled = sndReader.isLoopEnabled();
            level = sndReader.getLevel();
            tune = sndReader.getTune();
            beats = sndReader.getNumberOfBeats();
        }
        else
        {
            r.errorMessage = "Wrong file format";
            return;
        }
    }
    
    r.success = true;
    
    sound->setSampleRate(sampleRate);
    sound->setName(soundName);
    sound->setMono(mono);
    sound->setStart(start);
    sound->setEnd(end);
    sound->setLoopTo(loopTo);
    sound->setLoopEnabled(loopEnabled);
    sound->setLevel(level);
    sound->setTune(tune);
    sound->setBeatCount(beats);
    
    if (preview)
    {
        r.existingIndex = existingSoundIndex;
        return;
    }
    
    if (existingSoundIndex == -1)
    {
        if (partOfProgram)
            r.existingIndex = sampler->getSoundCount() - 1;
    }
    else
    {
        if (replace)
        {
            sampler->deleteSound(existingSoundIndex);
            sound->setMemoryIndex(existingSoundIndex);
            sampler->sort();
        }
        else
        {
            sampler->deleteSound((int)(sampler->getSoundCount()) - 1);
        }
        
        if (partOfProgram)
            r.existingIndex = existingSoundIndex;
    }
}

void SoundLoader::setPreview(bool b)
{
    preview = b;
}

double SoundLoader::logOfBase(float num, float base)
{
    return log(num) / log(base);
}

int SoundLoader::getSize()
{
    return size;
}
