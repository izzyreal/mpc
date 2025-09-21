#include <disk/SoundLoader.hpp>

#include <Mpc.hpp>
#include <disk/MpcFile.hpp>
#include <disk/AbstractDisk.hpp>
#include <file/wav/WavFile.hpp>
#include <file/sndreader/SndReader.hpp>
#include <sampler/Sound.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>

#include <StrUtil.hpp>

using namespace mpc::sampler;
using namespace mpc::disk;
using namespace mpc::file::wav;
using namespace mpc::file::sndreader;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SoundLoader::SoundLoader(mpc::Mpc& mpcToUse, bool replaceToUse)
: mpc (mpcToUse), replace (replaceToUse)
{
}

void SoundLoader::setPreview(bool b)
{
    preview = b;
}

void SoundLoader::loadSound(std::shared_ptr<MpcFile> f, SoundLoaderResult& r, std::shared_ptr<Sound> sound, const bool shouldBeConverted)
{
    const std::string soundFileName = f->getName();
    const std::string extension = f->getExtension();
    const std::string nameWithoutExtension = f->getNameWithoutExtension();
    
    auto sampler = mpc.getSampler();
 
    sound_or_error soundOrError;
  
    int16_t existingSoundIndex;

    if (StrUtil::eqIgnoreCase(extension, ".wav"))
    {
        existingSoundIndex = sampler->checkExists(nameWithoutExtension);
        bool willBeConverted = shouldBeConverted;
        wav_or_error wavMetaOrError = mpc.getDisk()->readWavMeta(f);
        
        if (!wavMetaOrError.has_value())
        {
            return;
        }

        auto wavMeta = wavMetaOrError.value();

        const auto bitDepth = wavMeta->getValidBits();
        const auto sampleRate = wavMeta->getSampleRate();

        if (bitDepth == 24 || bitDepth == 32 || sampleRate > 44100) {

            auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");

            if (!vmpcSettingsScreen->autoConvertWavs && !shouldBeConverted) {
                r.canBeConverted = true;
                return;
            }

            willBeConverted = true;
        }

        auto onSuccess = [&](std::shared_ptr<WavFile> wavFile){
            return onReadWavSuccess(wavFile, nameWithoutExtension, sound, willBeConverted);
        };

        soundOrError = mpc.getDisk()->readWav2(f, onSuccess);
    }
    else if (StrUtil::eqIgnoreCase(extension, ".snd"))
    {
        auto onSndReaderSuccess = [sampler, sound, nameWithoutExtension, &existingSoundIndex](std::shared_ptr<SndReader> sndReader) -> sound_or_error {

            if (!sndReader->isHeaderValid())
            {
                return tl::make_unexpected(mpc_io_error_msg{"Invalid SND header"});
            }

            if (StrUtil::eqIgnoreCase(sndReader->getName(), nameWithoutExtension))
            {
                existingSoundIndex = sampler->checkExists(sndReader->getName());
                sound->setName(sndReader->getName());
            }
            else
            {
                existingSoundIndex = sampler->checkExists(nameWithoutExtension);
                sound->setName(nameWithoutExtension);
            }

            sndReader->readData(sound->getMutableSampleData());
            sound->setMono(sndReader->isMono());
            sound->setStart(sndReader->getStart());
            sound->setEnd(sndReader->getEnd());
            sound->setLoopTo(sndReader->getEnd() - sndReader->getLoopLength());
            sound->setSampleRate(sndReader->getSampleRate());
            sound->setLoopEnabled(sndReader->isLoopEnabled());
            sound->setLevel(sndReader->getLevel());
            sound->setTune(sndReader->getTune());
            sound->setBeatCount(sndReader->getNumberOfBeats());
            return sound;
        };

        soundOrError = mpc.getDisk()->readSnd2(f, onSndReaderSuccess);
    }

    if (!soundOrError.has_value())
    {
        return;
    }
    
    r.success = true;
    
    if (preview)
    {
        return;
    }

    if (existingSoundIndex != -1)
    {
        if (replace)
        {
            auto existingSound = sampler->getSound(existingSoundIndex);

            soundOrError.map([existingSoundIndex, sampler, sound](std::shared_ptr<Sound> s) {
                sampler->replaceSound(existingSoundIndex, s);
            });
        }
        else
        {
            sampler->deleteSoundWithoutRepairingPrograms(sound);
        }
    }
}

sound_or_error SoundLoader::onReadWavSuccess(std::shared_ptr<mpc::file::wav::WavFile> &wavFile,
                                             std::string newSoundName,
                                             std::shared_ptr<mpc::sampler::Sound> sound,
                                             const bool shouldBeConverted)
{
    if (wavFile->getValidBits() != 16 && !shouldBeConverted) {
        wavFile->close();
        return tl::make_unexpected(mpc_io_error_msg{"Non-16bit,enable autoconvert"});
    }

    if ((wavFile->getSampleRate() < 8000 || wavFile->getSampleRate() > 44100) && !shouldBeConverted) {
        wavFile->close();
        return tl::make_unexpected(mpc_io_error_msg{"Samplerate has to be < 44100"});
    }

    sound->setName(newSoundName);

    int sampleRate = wavFile->getSampleRate();

    if (sampleRate > 44100 && shouldBeConverted)
    {
        sampleRate = 44100;
    }

    sound->setSampleRate(sampleRate);

    sound->setLevel(100);

    int numChannels = wavFile->getNumChannels();

    auto sampleData = sound->getMutableSampleData();

    if (numChannels == 1) {
        wavFile->readFrames(*sampleData, wavFile->getNumFrames());
    } else {
        std::vector<float> interleaved;
        wavFile->readFrames(interleaved, wavFile->getNumFrames());

        for (int i = 0; i < interleaved.size(); i += 2)
        {
            sampleData->push_back(interleaved[i]);
        }

        for (int i = 1; i < interleaved.size(); i += 2)
        {
            sampleData->push_back(interleaved[i]);
        }
    }

    if (wavFile->getSampleRate() > 44100 && shouldBeConverted) {
        auto tempSound = std::make_shared<Sound>(44100);
        tempSound->setMono(numChannels == 1);
        Sampler::resample(sampleData, wavFile->getSampleRate(), tempSound);
        auto tempData = tempSound->getMutableSampleData();
        sampleData->swap(*tempData);
    }

    sound->setMono(numChannels == 1);

    if (wavFile->getNumSampleLoops() > 0) {
        auto &sampleLoop = wavFile->getSampleLoop();
        const bool hasBeenConverted = wavFile->getSampleRate() > 44100 && shouldBeConverted;
        const float conversionRatio = wavFile->getSampleRate() / 44100.0;
        const auto sampleLoopStart = hasBeenConverted ? (sampleLoop.start / conversionRatio) : sampleLoop.start;
        sound->setLoopTo(sampleLoopStart);
        auto currentEnd = sound->getEnd();
        const auto sampleLoopEnd = hasBeenConverted ? (sampleLoop.end / conversionRatio) : sampleLoop.end;
        sound->setEnd(sampleLoopEnd <= 0 ? currentEnd : sampleLoopEnd);
        sound->setLoopEnabled(true);
    }

    if (sound->getLoopTo() == sound->getFrameCount())
    {
        sound->setLoopTo(0);
    }

    const auto tuneFactor = (float) (sound->getSampleRate() / 44100.0);
    const auto rateToTuneBase = (float) (pow(2, (1.0 / 12.0)));

    int tune = (int) (floor(log(tuneFactor) / log(rateToTuneBase) * 10.0));

    if (tune < -120)
        tune = -120;
    else if (tune > 120)
        tune = 120;

    sound->setTune(tune);
    return sound;
}
