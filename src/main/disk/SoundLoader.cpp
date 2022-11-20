#include <disk/SoundLoader.hpp>

#include <Mpc.hpp>
#include <disk/MpcFile.hpp>
#include <disk/AbstractDisk.hpp>
#include <file/wav/WavFile.hpp>
#include <sampler/Sound.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::sampler;
using namespace mpc::disk;
using namespace mpc::file::wav;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace moduru::lang;

SoundLoader::SoundLoader(mpc::Mpc& _mpc, std::vector<std::shared_ptr<mpc::sampler::Sound>> _sounds, bool _replace)
: mpc (_mpc), sounds (_sounds), replace (_replace)
{
}

void SoundLoader::setPartOfProgram(bool b)
{
    partOfProgram = b;
}

void SoundLoader::setPreview(bool b)
{
    preview = b;
}

void SoundLoader::loadSound(std::shared_ptr<MpcFile> f, SoundLoaderResult& r, std::shared_ptr<mpc::sampler::Sound> sound, bool shouldBeConverted)
{
    std::string soundFileName = f->getName();
    std::string extension = f->getExtension();
    std::string soundName = f->getNameWithoutExtension();
    
    auto sampler = mpc.getSampler();
    auto existingSoundIndex = sampler->checkExists(soundName);
 
    sound_or_error soundOrError;
    
    if (StrUtil::eqIgnoreCase(extension, ".wav"))
    {
        bool willBeConverted = shouldBeConverted;
        auto wavMeta = mpc.getDisk()->readWavMeta(f);
        
        if (!wavMeta.has_value())
        {
            r.errorMessage = wavMeta.error().log_msg;
            return;
        }
        
        wavMeta.map([&](mpc::file::wav::WavFile& wavFile) {
            
            const auto bitDepth = wavFile.getValidBits();
            const auto sampleRate = wavFile.getSampleRate();
            
            if (bitDepth == 24 || bitDepth == 32 || sampleRate > 44100) {
                
                auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
                
                if (!vmpcSettingsScreen->autoConvertWavs && !shouldBeConverted) {
                    r.canBeConverted = true;
                    return;
                } else {
                    willBeConverted = true;
                }
            }
        });
        
        soundOrError = mpc.getDisk()->readWav2(f, sound, willBeConverted);
    }
    else if (StrUtil::eqIgnoreCase(extension, ".snd"))
    {
        soundOrError = mpc.getDisk()->readSnd2(f, sound);
    }

    if (!soundOrError.has_value())
    {
        r.errorMessage = soundOrError.error().log_msg;
        return;
    }
    
    r.success = true;
    
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
            soundOrError.map([existingSoundIndex](std::shared_ptr<Sound> s) { s->setMemoryIndex(existingSoundIndex); });
            sampler->sort();
        }
        else
        {
            sampler->deleteSound(sound);
        }
        
        if (partOfProgram)
        {
            r.existingIndex = existingSoundIndex;
        }
    }
}
