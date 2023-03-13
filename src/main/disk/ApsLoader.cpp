#include "ApsLoader.hpp"
#include "lcdgui/screens/PgmAssignScreen.hpp"

#include <Mpc.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/MpcFile.hpp>
#include <disk/SoundLoader.hpp>
#include <file/aps/ApsNoteParameters.hpp>
#include <file/aps/ApsParser.hpp>
#include <file/aps/ApsSlider.hpp>

#include <sampler/NoteParameters.hpp>
#include <sampler/PgmSlider.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>
#include <lcdgui/screens/window/CantFindFileScreen.hpp>

#include <lang/StrUtil.hpp>

#include <stdexcept>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::disk;
using namespace mpc::sampler;
using namespace mpc::file::aps;
using namespace moduru::lang;

void ApsLoader::load(mpc::Mpc& mpc, std::shared_ptr<MpcFile> file)
{
    if (!file->exists())
        throw std::invalid_argument("File does not exist");
    
    auto cantFindFileScreen = mpc.screens->get<CantFindFileScreen>("cant-find-file");
    cantFindFileScreen->skipAll = false;

    ApsParser apsParser(mpc, file);
    
    if (!apsParser.isHeaderValid())
    {
        MLOG("The APS file you're trying to load does not have a valid ID. The first 2 bytes of an MPC2000XL APS file should be 0A 05. MPC2000 APS files start with 0A 04 and are not supported (yet?).");
        
        throw std::runtime_error("Invalid APS header");
    }
    
    auto withoutSounds = false;
    ApsLoader::loadFromParsedAps(apsParser, mpc, withoutSounds);
    
    mpc.getSampler()->setSoundIndex(0);
    
    mpc.getLayeredScreen()->openScreen("load");
}

void ApsLoader::loadFromParsedAps(ApsParser& apsParser, mpc::Mpc& mpc, bool headless, bool withoutSounds)
{
    auto sampler = mpc.getSampler();
    auto disk = mpc.getDisk();

    // These remain empty when loading the APS without sounds.
    // It is trusted that the caller of this method validates
    // sound indices to make sure they actually exist. This
    // allows loading APS and sounds in any order.
    std::vector<int> unavailableSoundIndices;
    std::map<int, int> finalSoundIndices;
    
    int skipCount = 0;
    
    if (!withoutSounds)
    {
        sampler->deleteAllSamples();
        
        for (int i = 0; i < apsParser.getSoundNames().size(); i++)
        {
            auto ext = "snd";
            std::shared_ptr<MpcFile> soundFile;
            std::string soundFileName = StrUtil::replaceAll(apsParser.getSoundNames()[i], ' ', "");
            
            for (auto& f : disk->getAllFiles())
            {
                if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".SND"))
                {
                    soundFile = f;
                    break;
                }
            }
            
            if (!soundFile || !soundFile->exists())
            {
                for (auto& f : disk->getAllFiles())
                {
                    if (StrUtil::eqIgnoreCase(StrUtil::replaceAll(f->getName(), ' ', ""), soundFileName + ".WAV"))
                    {
                        soundFile = f;
                        ext = "wav";
                        break;
                    }
                }
            }
            
            if (!soundFile || !soundFile->exists())
            {
                unavailableSoundIndices.push_back(i);
                                
                skipCount++;
                
                if (!headless)
                    ApsLoader::handleSoundNotFound(mpc, soundFileName);
                
                continue;
            }
            
            finalSoundIndices[i] = i - skipCount;
            
            ApsLoader::loadSound(mpc, soundFileName, ext, soundFile, headless);
        }
    }
    
    sampler->deleteAllPrograms();
    
    for (auto& apsProgram : apsParser.getPrograms())
    {
        auto newProgram = sampler->addProgram(apsProgram->index).lock();
        auto assignTable = apsProgram->getAssignTable()->get();
        
        newProgram->setName(apsProgram->getName());
        
        for (int noteIndex = 0; noteIndex < 64; noteIndex++)
        {
            newProgram->getPad(noteIndex)->setNote(assignTable[noteIndex]);
            
            auto sourceStereoMixerChannel = apsProgram->getStereoMixerChannel(noteIndex);
            auto sourceIndivFxMixerChannel = apsProgram->getIndivFxMixerChannel(noteIndex);
            
            auto destNoteParams = dynamic_cast<NoteParameters*>(newProgram->getNoteParameters(noteIndex + 35));
            auto destStereoMixerCh = destNoteParams->getStereoMixerChannel();
            auto destIndivFxCh = destNoteParams->getIndivFxMixerChannel();
            
            destIndivFxCh->setFxPath(sourceIndivFxMixerChannel.getFxPath());
            destStereoMixerCh->setLevel(sourceStereoMixerChannel.getLevel());
            destStereoMixerCh->setPanning(sourceStereoMixerChannel.getPanning());
            destIndivFxCh->setVolumeIndividualOut(sourceIndivFxMixerChannel.getVolumeIndividualOut());
            destIndivFxCh->setFxSendLevel(sourceIndivFxMixerChannel.getFxSendLevel());
            destIndivFxCh->setOutput(sourceIndivFxMixerChannel.getOutput());
            
            auto srcNoteParams = apsProgram->getNoteParameters(noteIndex);
            
            auto soundIndex = srcNoteParams->getSoundIndex();

            if (find(begin(unavailableSoundIndices), end(unavailableSoundIndices), soundIndex) != end(unavailableSoundIndices))
            {
                soundIndex = -1;
            }

            if (soundIndex != -1 && finalSoundIndices.find(soundIndex) != end(finalSoundIndices))
            {
                soundIndex = finalSoundIndices[soundIndex];
            }
            
            destNoteParams->setSoundIndex(soundIndex);
            destNoteParams->setTune(srcNoteParams->getTune());
            destNoteParams->setVoiceOverlap(srcNoteParams->getVoiceOverlap());
            destNoteParams->setDecayMode(srcNoteParams->getDecayMode());
            destNoteParams->setAttack(srcNoteParams->getAttack());
            destNoteParams->setDecay(srcNoteParams->getDecay());
            destNoteParams->setFilterAttack(srcNoteParams->getVelocityToFilterAttack());
            destNoteParams->setFilterDecay(srcNoteParams->getVelocityToFilterDecay());
            destNoteParams->setFilterEnvelopeAmount(srcNoteParams->getVelocityToFilterAmount());
            destNoteParams->setFilterFrequency(srcNoteParams->getCutoffFrequency());
            destNoteParams->setFilterResonance(srcNoteParams->getResonance());
            destNoteParams->setMuteAssignA(srcNoteParams->getMute1());
            destNoteParams->setMuteAssignB(srcNoteParams->getMute2());
            destNoteParams->setOptNoteA(srcNoteParams->getAlsoPlay1());
            destNoteParams->setOptionalNoteB(srcNoteParams->getAlsoPlay2());
            destNoteParams->setSliderParameterNumber(srcNoteParams->getSliderParameter());
            destNoteParams->setSoundGenMode(srcNoteParams->getSoundGenerationMode());
            destNoteParams->setVelocityToStart(srcNoteParams->getVelocityToStart());
            destNoteParams->setVelocityToAttack(srcNoteParams->getVelocityToAttack());
            destNoteParams->setVelocityToFilterFrequency(srcNoteParams->getVelocityToFilterFrequency());
            destNoteParams->setVeloToLevel(srcNoteParams->getVelocityToLevel());
            destNoteParams->setVeloRangeLower(srcNoteParams->getVelocityRangeLower());
            destNoteParams->setVeloRangeUpper(srcNoteParams->getVelocityRangeUpper());
            destNoteParams->setVelocityToPitch(srcNoteParams->getVelocityToPitch());
        }
        
        auto slider = dynamic_cast<mpc::sampler::PgmSlider*>(newProgram->getSlider());
        slider->setAttackHighRange(apsProgram->getSlider()->getAttackHigh());
        slider->setAttackLowRange(apsProgram->getSlider()->getAttackLow());
        slider->setControlChange(apsProgram->getSlider()->getProgramChange());
        slider->setDecayHighRange(apsProgram->getSlider()->getDecayHigh());
        slider->setDecayLowRange(apsProgram->getSlider()->getDecayLow());
        slider->setFilterHighRange(apsProgram->getSlider()->getFilterHigh());
        slider->setFilterLowRange(apsProgram->getSlider()->getFilterLow());
        slider->setAssignNote(apsProgram->getSlider()->getNote());
        slider->setTuneHighRange(apsProgram->getSlider()->getTuneHigh());
        slider->setTuneLowRange(apsProgram->getSlider()->getTuneLow());
    }
    
    for (int i = 0; i < 4; i++)
    {
        auto m = apsParser.getDrumMixers()[i];
        auto& drum = mpc.getDrum(i);
        
        for (int noteIndex = 0; noteIndex < 64; noteIndex++)
        {
            auto apssmc = m->getStereoMixerChannel(noteIndex);
            auto apsifmc = m->getIndivFxMixerChannel(noteIndex);
            auto drumsmc = drum.getStereoMixerChannels()[noteIndex];
            auto drumifmc = drum.getIndivFxMixerChannels()[noteIndex];

            drumifmc->setFxPath(apsifmc.getFxPath());
            drumsmc->setLevel(apssmc.getLevel());
            drumsmc->setPanning(apssmc.getPanning());
            drumifmc->setVolumeIndividualOut(apsifmc.getVolumeIndividualOut());
            drumifmc->setOutput(apsifmc.getOutput());
            drumifmc->setFxSendLevel(apsifmc.getFxSendLevel());
        }
        
        auto pgm = apsParser.getDrumConfiguration(i)->getProgram();
        drum.setProgram(pgm);
        drum.setReceivePgmChange(apsParser.getDrumConfiguration(i)->getReceivePgmChange());
        drum.setReceiveMidiVolume(apsParser.getDrumConfiguration(i)->getReceiveMidiVolume());
    }
    
    auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
    
    auto globals = apsParser.getGlobalParameters();
    
    mixerSetupScreen->setRecordMixChangesEnabled(globals->isRecordMixChangesEnabled());
    mixerSetupScreen->setCopyPgmMixToDrumEnabled(globals->isCopyPgmMixToDrumEnabled());
    mixerSetupScreen->setFxDrum(globals->getFxDrum());
    mixerSetupScreen->setIndivFxSourceDrum(globals->isIndivFxSourceDrum());
    mixerSetupScreen->setStereoMixSourceDrum(globals->isStereoMixSourceDrum());
    
    auto drumScreen = mpc.screens->get<DrumScreen>("drum");
    drumScreen->setPadToIntSound(globals->isPadToIntSoundEnabled());
    mixerSetupScreen->setMasterLevel(globals->getMasterLevel());

    auto pgmAssignScreen = mpc.screens->get<PgmAssignScreen>("program-assign");

    pgmAssignScreen->setPadAssign(globals->isPadAssignMaster());
}

void ApsLoader::loadSound(mpc::Mpc& mpc,
                          std::string soundFileName,
                          std::string ext,
                          std::weak_ptr<MpcFile> _soundFile,
                          bool headless)
{
    auto soundFile = _soundFile.lock();
    const auto replace = false;
    SoundLoader soundLoader(mpc, replace);

    if (!headless)
    {
        ApsLoader::showPopup(mpc, soundFileName, ext, soundFile->length());
    }
    
    SoundLoaderResult result;
    bool shouldBeConverted = false;
    auto sound = mpc.getSampler()->addSound();

    soundLoader.loadSound(soundFile, result, sound, shouldBeConverted);

    if  (!result.success)
    {
        mpc.getSampler()->deleteSound(sound);
    }
}

void ApsLoader::showPopup(mpc::Mpc& mpc, std::string name, std::string ext, int sampleSize)
{
    mpc.getLayeredScreen()->openScreen("popup");
    auto popupScreen = mpc.screens->get<PopupScreen>("popup");
    popupScreen->setText("LOADING " + StrUtil::toUpper(StrUtil::padRight(name, " ", 16) + "." + ext));
    
    if (std::dynamic_pointer_cast<StdDisk>(mpc.getDisk()))
    {
        auto sleepTime = sampleSize / 800;
        
        if (sleepTime < 300)
            sleepTime = 300;
        
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(sleepTime * 0.2)));
    }
}

void ApsLoader::handleSoundNotFound(mpc::Mpc &mpc, std::string soundFileName)
{
    auto cantFindFileScreen = mpc.screens->get<CantFindFileScreen>("cant-find-file");
    auto skipAll = cantFindFileScreen->skipAll;
    
    if (!skipAll)
    {
        cantFindFileScreen->waitingForUser = true;
        
        cantFindFileScreen->fileName = soundFileName;
        
        mpc.getLayeredScreen()->openScreen("cant-find-file");
        
        while (cantFindFileScreen->waitingForUser)
            std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
}
