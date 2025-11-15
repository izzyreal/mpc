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

#include "sampler/NoteParameters.hpp"
#include "sampler/PgmSlider.hpp"

#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "lcdgui/screens/window/CantFindFileScreen.hpp"

#include "StrUtil.hpp"

#include <stdexcept>

#include "Logger.hpp"
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

void ApsLoader::load(Mpc &mpc, const std::shared_ptr<MpcFile> &file)
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
    loadFromParsedAps(apsParser, mpc, withoutSounds);

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

    sampler->deleteAllPrograms(/*createDefaultProgram=*/false);

    for (auto &apsProgram : apsParser.getPrograms())
    {
        auto newProgram = sampler->addProgram(apsProgram->index).lock();
        auto assignTable = apsProgram->getAssignTable()->get();

        newProgram->setName(apsProgram->getName());

        for (int noteIndex = 0; noteIndex < 64; noteIndex++)
        {
            newProgram->getPad(noteIndex)->setNote(
                DrumNoteNumber(assignTable[noteIndex]));

            auto sourceStereoMixerChannel =
                apsProgram->getStereoMixerChannel(noteIndex);
            auto sourceIndivFxMixerChannel =
                apsProgram->getIndivFxMixerChannel(noteIndex);

            auto destNoteParams = newProgram->getNoteParameters(noteIndex + 35);
            auto destStereoMixerCh = destNoteParams->getStereoMixerChannel();
            auto destIndivFxCh = destNoteParams->getIndivFxMixerChannel();

            destIndivFxCh->setFxPath(sourceIndivFxMixerChannel.getFxPath());
            destStereoMixerCh->setLevel(sourceStereoMixerChannel.getLevel());
            destStereoMixerCh->setPanning(
                sourceStereoMixerChannel.getPanning());
            destIndivFxCh->setVolumeIndividualOut(
                sourceIndivFxMixerChannel.getVolumeIndividualOut());
            destIndivFxCh->setFxSendLevel(
                sourceIndivFxMixerChannel.getFxSendLevel());
            destIndivFxCh->setOutput(sourceIndivFxMixerChannel.getOutput());

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

            destNoteParams->setSoundIndex(soundIndex);
            destNoteParams->setTune(srcNoteParams->getTune());
            destNoteParams->setVoiceOverlapMode(
                srcNoteParams->getVoiceOverlapMode());
            destNoteParams->setDecayMode(srcNoteParams->getDecayMode());
            destNoteParams->setAttack(srcNoteParams->getAttack());
            destNoteParams->setDecay(srcNoteParams->getDecay());
            destNoteParams->setFilterAttack(
                srcNoteParams->getVelocityToFilterAttack());
            destNoteParams->setFilterDecay(
                srcNoteParams->getVelocityToFilterDecay());
            destNoteParams->setFilterEnvelopeAmount(
                srcNoteParams->getVelocityToFilterAmount());
            destNoteParams->setFilterFrequency(
                srcNoteParams->getCutoffFrequency());
            destNoteParams->setFilterResonance(srcNoteParams->getResonance());
            destNoteParams->setMuteAssignA(
                DrumNoteNumber(srcNoteParams->getMute1()));
            destNoteParams->setMuteAssignB(
                DrumNoteNumber(srcNoteParams->getMute2()));
            destNoteParams->setOptNoteA(
                DrumNoteNumber(srcNoteParams->getAlsoPlay1()));
            destNoteParams->setOptionalNoteB(
                DrumNoteNumber(srcNoteParams->getAlsoPlay2()));
            destNoteParams->setSliderParameterNumber(
                srcNoteParams->getSliderParameter());
            destNoteParams->setSoundGenMode(
                srcNoteParams->getSoundGenerationMode());
            destNoteParams->setVelocityToStart(
                srcNoteParams->getVelocityToStart());
            destNoteParams->setVelocityToAttack(
                srcNoteParams->getVelocityToAttack());
            destNoteParams->setVelocityToFilterFrequency(
                srcNoteParams->getVelocityToFilterFrequency());
            destNoteParams->setVeloToLevel(srcNoteParams->getVelocityToLevel());
            destNoteParams->setVeloRangeLower(
                srcNoteParams->getVelocityRangeLower());
            destNoteParams->setVeloRangeUpper(
                srcNoteParams->getVelocityRangeUpper());
            destNoteParams->setVelocityToPitch(
                srcNoteParams->getVelocityToPitch());
        }

        auto slider = newProgram->getSlider();
        slider->setAttackHighRange(apsProgram->getSlider()->getAttackHigh());
        slider->setAttackLowRange(apsProgram->getSlider()->getAttackLow());
        slider->setControlChange(apsProgram->getSlider()->getProgramChange());
        slider->setDecayHighRange(apsProgram->getSlider()->getDecayHigh());
        slider->setDecayLowRange(apsProgram->getSlider()->getDecayLow());
        slider->setFilterHighRange(apsProgram->getSlider()->getFilterHigh());
        slider->setFilterLowRange(apsProgram->getSlider()->getFilterLow());
        slider->setAssignNote(
            DrumNoteNumber(apsProgram->getSlider()->getNote()));
        slider->setTuneHighRange(apsProgram->getSlider()->getTuneHigh());
        slider->setTuneLowRange(apsProgram->getSlider()->getTuneLow());
    }

    for (int i = 0; i < Mpc2000XlSpecs::DRUM_BUS_COUNT; i++)
    {
        auto m = apsParser.getDrumMixers()[i];
        auto drum = mpc.getSequencer()->getDrumBus(DrumBusIndex(i));

        for (int noteIndex = 0; noteIndex < 64; noteIndex++)
        {
            auto apssmc = m->getStereoMixerChannel(noteIndex);
            auto apsifmc = m->getIndivFxMixerChannel(noteIndex);
            auto drumsmc = drum->getStereoMixerChannels()[noteIndex];
            auto drumifmc = drum->getIndivFxMixerChannels()[noteIndex];

            drumifmc->setFxPath(apsifmc.getFxPath());
            drumsmc->setLevel(apssmc.getLevel());
            drumsmc->setPanning(apssmc.getPanning());
            drumifmc->setVolumeIndividualOut(apsifmc.getVolumeIndividualOut());
            drumifmc->setOutput(apsifmc.getOutput());
            drumifmc->setFxSendLevel(apsifmc.getFxSendLevel());
        }

        auto pgm = apsParser.getDrumConfiguration(i)->getProgram();
        drum->setProgram(ProgramIndex(pgm));
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
        "LOADING " +
        StrUtil::toUpper(StrUtil::padRight(name, " ", 16) + "." + ext);
    auto ls = mpc.getLayeredScreen();
    ls->postToUiThread([ls, msg]
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

        ls->postToUiThread([ls, cantFindFileScreen, soundFileName]{
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
