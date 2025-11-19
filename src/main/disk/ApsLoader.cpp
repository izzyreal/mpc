#include "ApsLoader.hpp"
#include "lcdgui/screens/PgmAssignScreen.hpp"
#include "sampler/Pad.hpp"

#include "Mpc.hpp"
#include "disk/StdDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/SoundLoader.hpp"
#include "file/aps/ApsNoteParameters.hpp"
#include "file/aps/ApsParser.hpp"
#include "file/aps/ApsSlider.hpp"

#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "lcdgui/screens/window/CantFindFileScreen.hpp"

#include "StrUtil.hpp"

#include <stdexcept>

#include "Logger.hpp"
#include "engine/EngineHost.hpp"
#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::disk;
using namespace mpc::sampler;
using namespace mpc::file::aps;

void ApsLoader::load(Mpc &mpc, const std::shared_ptr<MpcFile> &file,
                     const bool headless)
{
    if (!file->exists())
    {
        throw std::invalid_argument("File does not exist");
    }

    const auto seqScreen = mpc.screens->get<ScreenId::DrumScreen>();

    const auto cantFindFileScreen =
        mpc.screens->get<ScreenId::CantFindFileScreen>();
    cantFindFileScreen->skipAll = false;

    ApsParser apsParser(file->getBytes());

    if (!apsParser.isHeaderValid())
    {
        MLOG(
            "The APS file you're trying to load does not have a valid ID. The "
            "first 2 bytes of an MPC2000XL APS file should be 0A 05. MPC2000 "
            "APS files start with 0A 04 and are not supported (yet?).");

        throw std::runtime_error("Invalid APS header");
    }

    constexpr auto withoutSounds = false;
    loadFromParsedAps(apsParser, mpc, withoutSounds, headless);

    mpc.getSampler()->setSoundIndex(0);
}

void ApsLoader::loadFromParsedAps(ApsParser &apsParser, Mpc &mpc,
                                  bool withoutSounds, bool headless)
{
    auto sampler = mpc.getSampler();
    auto disk = mpc.getDisk();

    // These remain empty when loading the APS without sounds.
    // It is trusted that the caller of this method validates
    // sound indices to make sure they actually exist. This
    // allows loading APS and sounds in any order.
    std::vector<int> unavailableSoundIndices;
    std::map<int, int> finalSoundIndices;

    if (!withoutSounds)
    {
        int skipCount = 0;
        sampler->deleteAllSamples();

        for (int i = 0; i < apsParser.getSoundNames().size(); i++)
        {
            auto ext = "snd";
            std::shared_ptr<MpcFile> soundFile;
            std::string soundFileName =
                StrUtil::replaceAll(apsParser.getSoundNames()[i], ' ', "");

            for (auto &f : disk->getAllFiles())
            {
                if (StrUtil::eqIgnoreCase(
                        StrUtil::replaceAll(f->getName(), ' ', ""),
                        soundFileName + ".SND"))
                {
                    soundFile = f;
                    break;
                }
            }

            if (!soundFile || !soundFile->exists())
            {
                for (auto &f : disk->getAllFiles())
                {
                    if (StrUtil::eqIgnoreCase(
                            StrUtil::replaceAll(f->getName(), ' ', ""),
                            soundFileName + ".WAV"))
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
                {
                    handleSoundNotFound(mpc, soundFileName);
                }

                continue;
            }

            finalSoundIndices[i] = i - skipCount;

            loadSound(mpc, soundFileName, ext, soundFile, headless);
        }
    }

    std::array<performance::Program, Mpc2000XlSpecs::MAX_PROGRAM_COUNT>
        perfPrograms{};

    for (auto &apsProgram : apsParser.getPrograms())
    {
        sampler->getProgram(apsProgram->index)->setName(apsProgram->getName());

        auto &perfProgram = perfPrograms[apsProgram->index];
        perfProgram.used = true;

        auto assignTable = apsProgram->getAssignTable()->get();

        for (int noteIndex = 0; noteIndex < 64; noteIndex++)
        {
            perfProgram.pads[noteIndex].note =
                DrumNoteNumber(assignTable[noteIndex]);

            auto sourceStereoMixerChannel =
                apsProgram->getStereoMixerChannel(noteIndex);
            auto sourceIndivFxMixerChannel =
                apsProgram->getIndivFxMixerChannel(noteIndex);

            auto &destNoteParams = perfProgram.noteParameters[noteIndex];
            destNoteParams.stereoMixer = sourceStereoMixerChannel;
            destNoteParams.indivFxMixer = sourceIndivFxMixerChannel;

            auto srcNoteParams = apsProgram->getNoteParameters(noteIndex);

            auto soundIndex = srcNoteParams->getSoundIndex();

            if (find(begin(unavailableSoundIndices),
                     end(unavailableSoundIndices),
                     soundIndex) != end(unavailableSoundIndices))
            {
                soundIndex = -1;
            }

            if (soundIndex != -1 &&
                finalSoundIndices.find(soundIndex) != end(finalSoundIndices))
            {
                soundIndex = finalSoundIndices[soundIndex];
            }

            destNoteParams.soundIndex = soundIndex;
            destNoteParams.tune = srcNoteParams->getTune();
            destNoteParams.voiceOverlapMode =
                srcNoteParams->getVoiceOverlapMode();

            destNoteParams.decayMode = srcNoteParams->getDecayMode();
            destNoteParams.attack = srcNoteParams->getAttack();
            destNoteParams.decay = srcNoteParams->getDecay();

            destNoteParams.filterAttack =
                srcNoteParams->getVelocityToFilterAttack();
            destNoteParams.filterDecay =
                srcNoteParams->getVelocityToFilterDecay();
            destNoteParams.filterEnvelopeAmount =
                srcNoteParams->getVelocityToFilterAmount();

            destNoteParams.filterFrequency =
                srcNoteParams->getCutoffFrequency();
            destNoteParams.filterResonance = srcNoteParams->getResonance();

            destNoteParams.muteAssignA =
                DrumNoteNumber(srcNoteParams->getMute1());
            destNoteParams.muteAssignB =
                DrumNoteNumber(srcNoteParams->getMute2());

            destNoteParams.optionalNoteA =
                DrumNoteNumber(srcNoteParams->getAlsoPlay1());
            destNoteParams.optionalNoteB =
                DrumNoteNumber(srcNoteParams->getAlsoPlay2());

            destNoteParams.sliderParameterNumber =
                srcNoteParams->getSliderParameter();
            destNoteParams.soundGenerationMode =
                srcNoteParams->getSoundGenerationMode();

            destNoteParams.velocityToStart =
                srcNoteParams->getVelocityToStart();
            destNoteParams.velocityToAttack =
                srcNoteParams->getVelocityToAttack();
            destNoteParams.velocityToFilterFrequency =
                srcNoteParams->getVelocityToFilterFrequency();

            destNoteParams.velocityToLevel =
                srcNoteParams->getVelocityToLevel();

            destNoteParams.velocityRangeLower =
                srcNoteParams->getVelocityRangeLower();
            destNoteParams.velocityRangeUpper =
                srcNoteParams->getVelocityRangeUpper();

            destNoteParams.velocityToPitch =
                srcNoteParams->getVelocityToPitch();
        }

        perfProgram.slider.attackHighRange =
            apsProgram->getSlider()->getAttackHigh();
        perfProgram.slider.attackLowRange =
            apsProgram->getSlider()->getAttackLow();
        perfProgram.slider.controlChange =
            apsProgram->getSlider()->getProgramChange();
        perfProgram.slider.decayHighRange =
            apsProgram->getSlider()->getDecayHigh();
        perfProgram.slider.decayLowRange =
            apsProgram->getSlider()->getDecayLow();
        perfProgram.slider.filterHighRange =
            apsProgram->getSlider()->getFilterHigh();
        perfProgram.slider.filterLowRange =
            apsProgram->getSlider()->getFilterLow();
        perfProgram.slider.assignNote =
            DrumNoteNumber(apsProgram->getSlider()->getNote());
        perfProgram.slider.tuneHighRange =
            apsProgram->getSlider()->getTuneHigh();
        perfProgram.slider.tuneLowRange = apsProgram->getSlider()->getTuneLow();
    }

    performance::UpdateProgramsBulk payload{perfPrograms};
    mpc.performanceManager->enqueue(
        performance::PerformanceMessage(std::move(payload)));

    for (int i = 0; i < Mpc2000XlSpecs::DRUM_BUS_COUNT; i++)
    {
        auto m = apsParser.getDrumMixers()[i];
        auto drum = mpc.getSequencer()->getDrumBus(DrumBusIndex(i));

        for (int noteIndex = 0; noteIndex < 64; noteIndex++)
        {
            auto apsStereoMixer = m->getStereoMixerChannel(noteIndex);
            auto apsIndivFxMixer = m->getIndivFxMixerChannel(noteIndex);
            auto drumStereoMixer = drum->getStereoMixerChannels()[noteIndex];
            auto drumIndivFxMixer = drum->getIndivFxMixerChannels()[noteIndex];

            drumIndivFxMixer->setFxPath(apsIndivFxMixer.fxPath);
            drumStereoMixer->setLevel(apsStereoMixer.level);
            drumStereoMixer->setPanning(apsStereoMixer.panning);
            drumIndivFxMixer->setVolumeIndividualOut(
                apsIndivFxMixer.individualOutLevel);
            drumIndivFxMixer->setOutput(apsIndivFxMixer.individualOutput);
            drumIndivFxMixer->setFxSendLevel(apsIndivFxMixer.fxSendLevel);
        }

        auto pgm = apsParser.getDrumConfiguration(i)->getProgram();
        drum->setProgramIndex(ProgramIndex(pgm));
        drum->setReceivePgmChange(
            apsParser.getDrumConfiguration(i)->getReceivePgmChange());
        drum->setReceiveMidiVolume(
            apsParser.getDrumConfiguration(i)->getReceiveMidiVolume());
    }

    auto mixerSetupScreen = mpc.screens->get<ScreenId::MixerSetupScreen>();

    auto globals = apsParser.getGlobalParameters();

    mixerSetupScreen->setRecordMixChangesEnabled(
        globals->isRecordMixChangesEnabled());
    mixerSetupScreen->setCopyPgmMixToDrumEnabled(
        globals->isCopyPgmMixToDrumEnabled());
    mixerSetupScreen->setFxDrum(globals->getFxDrum());
    mixerSetupScreen->setIndivFxSourceDrum(globals->isIndivFxSourceDrum());
    mixerSetupScreen->setStereoMixSourceDrum(globals->isStereoMixSourceDrum());

    auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
    drumScreen->setPadToIntSound(globals->isPadToIntSoundEnabled());
    mixerSetupScreen->setMasterLevel(globals->getMasterLevel());

    auto pgmAssignScreen = mpc.screens->get<ScreenId::PgmAssignScreen>();

    pgmAssignScreen->setPadAssign(globals->isPadAssignMaster());
}

void ApsLoader::loadSound(Mpc &mpc, const std::string &soundFileName,
                          const std::string &ext,
                          const std::shared_ptr<MpcFile> &soundFile,
                          const bool headless)
{
    constexpr auto replace = false;
    SoundLoader soundLoader(mpc, replace);

    if (!headless)
    {
        showPopup(mpc, soundFileName, ext);
    }

    SoundLoaderResult result;
    constexpr bool shouldBeConverted = false;
    const auto sound = mpc.getSampler()->addSound();

    if (!sound)
    {
        return;
    }

    soundLoader.loadSound(soundFile, result, sound, shouldBeConverted);

    if (!result.success)
    {
        mpc.getSampler()->deleteSound(sound);
    }
}

void ApsLoader::showPopup(Mpc &mpc, const std::string &name,
                          const std::string &ext)
{
    const std::string msg =
        "Loading " +
        StrUtil::toUpper(StrUtil::padRight(name, " ", 16) + "." + ext);
    auto ls = mpc.getLayeredScreen();
    ls->postToUiThread(
        [ls, msg]
        {
            ls->showPopup(msg);
        });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void ApsLoader::handleSoundNotFound(Mpc &mpc, const std::string &soundFileName)
{
    const auto cantFindFileScreen =
        mpc.screens->get<ScreenId::CantFindFileScreen>();

    if (const auto skipAll = cantFindFileScreen->skipAll; !skipAll)
    {
        auto ls = mpc.getLayeredScreen();

        ls->postToUiThread(
            [ls, cantFindFileScreen, soundFileName]
            {
                cantFindFileScreen->waitingForUser = true;
                cantFindFileScreen->fileName = soundFileName;
                ls->openScreenById(ScreenId::CantFindFileScreen);
            });

        while (cantFindFileScreen->waitingForUser)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}
