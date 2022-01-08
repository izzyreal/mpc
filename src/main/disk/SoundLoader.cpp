#include <disk/SoundLoader.hpp>

#include <Mpc.hpp>
#include <disk/MpcFile.hpp>
#include <disk/AbstractDisk.hpp>
#include <file/sndreader/SndReader.hpp>
#include <file/wav/WavFile.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>
#include <lcdgui/screens/VmpcSettingsScreen.hpp>

#include <lang/StrUtil.hpp>

#include <cmath>

using namespace mpc::sampler;
using namespace mpc::disk;
using namespace mpc::file::wav;
using namespace mpc::file::sndreader;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

SoundLoader::SoundLoader(mpc::Mpc& _mpc)
: mpc (_mpc)
{
}

SoundLoader::SoundLoader(mpc::Mpc& _mpc, vector<weak_ptr<mpc::sampler::Sound>> _sounds, bool _replace)
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

void SoundLoader::loadSound(shared_ptr<MpcFile> f, SoundLoaderResult& r, bool shouldBeConverted)
{
    string soundFileName = f->getName();
    string extension = f->getExtension();
    string soundName = f->getNameWithoutExtension();
    
    auto sampler = mpc.getSampler().lock();
    auto existingSoundIndex = sampler->checkExists(soundName);
 
    sound_or_error sound;
    
    if (StrUtil::eqIgnoreCase(extension, ".wav"))
    {
        bool willBeConverted = shouldBeConverted;
        auto wavMeta = mpc.getDisk().lock()->readWavMeta(f);
        
        if (!wavMeta.has_value()) return;
        
        wavMeta.map([&](mpc::file::wav::WavFile wavFile) {
            
            const auto bitDepth = wavFile.getValidBits();
            const auto sampleRate = wavFile.getSampleRate();
            
            if (bitDepth == 24 || bitDepth == 32 || sampleRate > 48000) {
                
                auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");
                
                if (!vmpcSettingsScreen->autoConvertWavs && !shouldBeConverted) {
                    r.canBeConverted = true;
                    return;
                } else {
                    willBeConverted = true;
                }
            }
        });
        
        sound = mpc.getDisk().lock()->readWav2(f, willBeConverted);
    }
    else if (StrUtil::eqIgnoreCase(extension, ".snd"))
    {
        sound = mpc.getDisk().lock()->readSnd2(f);
    }
    
    if (!sound.has_value()) return;
    
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
            sound.map([existingSoundIndex](std::shared_ptr<Sound> s) { s->setMemoryIndex(existingSoundIndex); });
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
