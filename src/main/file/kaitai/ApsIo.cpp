#include "file/kaitai/ApsIo.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/SoundLoader.hpp"
#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"
#include "file/BitUtil.hpp"
#include "file/ByteUtil.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/mpc2000xl_aps.h"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "lcdgui/screens/PgmAssignScreen.hpp"
#include "lcdgui/screens/window/CantFindFileScreen.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/PgmSlider.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

#include <algorithm>
#include <chrono>
#include <map>
#include <sstream>
#include <stdexcept>
#include <thread>

#include <kaitai/kaitaistream.h>

namespace {

constexpr uint8_t kUnassignedSoundIndex = 0xFF;
constexpr int kOffNoteNumber = 34;
constexpr int kVisibleNameLength = 16;
constexpr int kProgramPadCount = 64;
constexpr char kApsProgramUnknownBytes[] = {0, 7, 4, 30, 0};
constexpr char kApsProgramSliderPadding[] = {35, 64, 0, 26, 0};
constexpr char kApsProgramNoteTerminator = 6;
constexpr char kApsProgramMixerPadding[] = {0, 64, 0};
constexpr char kApsTopLevelPadAfterSounds[] = {24, 0};
constexpr char kApsDrumPadBeforeFirstDrum[] = {4, 0, static_cast<char>(136), 1, 64, 0, 6};
constexpr char kApsAfterDrums[] = {1, 127};
constexpr char kApsEndMarker[] = {static_cast<char>(255), static_cast<char>(255)};
constexpr char kApsGlobalTemplateNoSounds[] = {127, static_cast<char>(254), 124, static_cast<char>(238), 0, 0, 0, 64};
constexpr char kApsGlobalTemplateWithSounds[] = {3, 56, 0, static_cast<char>(161), 0, 0, 0, 64};
constexpr char kApsDrumConfigTemplate[] = {0, 0, 1, 1, 127, 0, 1, 1, 127, 64, 0, 6};
constexpr char kPadTailFxBoardSettingsHex[] =
    "02004800D0070000630114081DFC323302323C08050A1414320000020F19000541141E0105"
    "0000056300F4FF0C0000000000000002004F014F0100424F0100424F01004232006328003C"
    "0000D0070000630114081DFC323302323C08050A1414320000020F19000541141E01050000"
    "056300F4FF0C0000000000000002004F014F0100424F0100424F01004232006328003C0000"
    "04000C000000320023003E335A3214000000320023003E335A3214000000320023003E335A"
    "3214000000320023003E335A321400";

std::string stripNullTerminatedField(const std::string& raw)
{
    const auto terminator = raw.find('\0');
    return terminator == std::string::npos ? raw : raw.substr(0, terminator);
}

std::string trimNullTerminatedField(const std::string& raw)
{
    return mpc::StrUtil::trim(stripNullTerminatedField(raw));
}

int normalizeOffNote(const uint8_t raw)
{
    return raw == 0 ? kOffNoteNumber : raw;
}

void copyMixer(mpc::performance::StereoMixer& stereoDest,
               mpc::performance::IndivFxMixer& indivDest,
               const mpc2000xl_pgm_t::pad_mixer_t& mixer)
{
    stereoDest.level = mpc::DrumMixerLevel(mixer.volume());
    stereoDest.panning = mpc::DrumMixerPanning(mixer.pan());
    indivDest.individualOutLevel = mpc::DrumMixerLevel(mixer.volume_individual());
    indivDest.individualOutput = mpc::DrumMixerIndividualOutput(mixer.output());
    indivDest.fxSendLevel = mpc::DrumMixerLevel(mixer.effects_send_level());
    indivDest.fxPath = mpc::DrumMixerIndividualFxPath(mixer.fx_output());
}

void appendBytes(std::vector<char>& dest, const std::vector<char>& src)
{
    dest.insert(dest.end(), src.begin(), src.end());
}

void appendBytes(std::vector<char>& dest, const std::string& src)
{
    dest.insert(dest.end(), src.begin(), src.end());
}

void appendBytes(std::vector<char>& dest, const char* src, const std::size_t size)
{
    dest.insert(dest.end(), src, src + size);
}

std::vector<char> toNullTerminatedAsciiBytes(const std::string& value)
{
    std::vector<char> bytes(kVisibleNameLength + 1, ' ');
    const auto trimmed = value.substr(0, kVisibleNameLength);
    std::copy(trimmed.begin(), trimmed.end(), bytes.begin());
    bytes.back() = 0;
    return bytes;
}

std::vector<char> buildGlobalParametersBytes(mpc::Mpc& mpc, const int soundCount)
{
    std::vector<char> bytes(
        soundCount == 0
            ? std::begin(kApsGlobalTemplateNoSounds)
            : std::begin(kApsGlobalTemplateWithSounds),
        soundCount == 0
            ? std::end(kApsGlobalTemplateNoSounds)
            : std::end(kApsGlobalTemplateWithSounds));

    auto drumScreen = mpc.screens->get<mpc::lcdgui::ScreenId::DrumScreen>();
    auto pgmAssignScreen = mpc.screens->get<mpc::lcdgui::ScreenId::PgmAssignScreen>();
    auto mixerSetupScreen = mpc.screens->get<mpc::lcdgui::ScreenId::MixerSetupScreen>();

    bytes[0] = mpc::file::BitUtil::setBits(1, bytes[0], drumScreen->isPadToIntSound());
    bytes[1] = mpc::file::BitUtil::setBits(1, bytes[1], pgmAssignScreen->isPadAssignMaster());
    bytes[2] = mpc::file::BitUtil::setBits(1, bytes[2], mixerSetupScreen->isStereoMixSourceDrum());
    bytes[2] = mpc::file::BitUtil::setBits(2, bytes[2], mixerSetupScreen->isIndivFxSourceDrum());
    bytes[3] = mpc::file::BitUtil::setBits(1, bytes[3], mixerSetupScreen->isCopyPgmMixToDrumEnabled());
    bytes[3] = mpc::file::BitUtil::setBits(16, bytes[3], mixerSetupScreen->isRecordMixChangesEnabled());
    bytes[4] = static_cast<char>(mixerSetupScreen->getFxDrum());
    bytes[6] = static_cast<char>(mixerSetupScreen->getMasterLevel());
    return bytes;
}

std::vector<char> buildDrumMixerBytes(const std::shared_ptr<mpc::sequencer::DrumBus>& drumBus)
{
    std::vector<char> bytes;
    bytes.reserve(kProgramPadCount * 6);

    const auto stereoMixers = drumBus->getStereoMixerChannels();
    const auto indivFxMixers = drumBus->getIndivFxMixerChannels();

    for (int i = 0; i < kProgramPadCount; ++i)
    {
        const auto& stereo = stereoMixers[i];
        const auto& indiv = indivFxMixers[i];
        bytes.push_back(static_cast<char>(indiv->getFxPath()));
        bytes.push_back(static_cast<char>(stereo->getLevel()));
        bytes.push_back(static_cast<char>(stereo->getPanning()));
        bytes.push_back(static_cast<char>(indiv->getVolumeIndividualOut()));
        bytes.push_back(static_cast<char>(indiv->getOutput()));
        bytes.push_back(static_cast<char>(indiv->getFxSendLevel()));
    }

    return bytes;
}

std::vector<char> buildDrumConfigBytes(const std::shared_ptr<mpc::sequencer::DrumBus>& drumBus,
                                       const bool fullLength)
{
    std::vector<char> bytes(std::begin(kApsDrumConfigTemplate), std::end(kApsDrumConfigTemplate));
    bytes[1] = static_cast<char>(drumBus->getProgramIndex());
    bytes[2] = mpc::file::BitUtil::setBits(1, bytes[2], drumBus->receivesPgmChange());
    bytes[3] = mpc::file::BitUtil::setBits(1, bytes[3], drumBus->receivesMidiVolume());
    bytes[5] = static_cast<char>(drumBus->getProgramIndex());

    if (!fullLength)
    {
        bytes.resize(7);
    }

    return bytes;
}

std::vector<char> buildProgramBytes(mpc::sampler::Program& program, const int index)
{
    std::vector<char> bytes;
    bytes.reserve(2350);

    bytes.push_back(static_cast<char>(index));
    appendBytes(bytes, kApsProgramUnknownBytes, sizeof(kApsProgramUnknownBytes));
    appendBytes(bytes, toNullTerminatedAsciiBytes(program.getName()));

    const auto* slider = program.getSlider();
    bytes.push_back(static_cast<char>(slider->getNote() == 34 ? 0 : slider->getNote().get()));
    bytes.push_back(static_cast<char>(slider->getTuneLowRange()));
    bytes.push_back(static_cast<char>(slider->getTuneHighRange()));
    bytes.push_back(static_cast<char>(slider->getDecayLowRange()));
    bytes.push_back(static_cast<char>(slider->getDecayHighRange()));
    bytes.push_back(static_cast<char>(slider->getAttackLowRange()));
    bytes.push_back(static_cast<char>(slider->getAttackHighRange()));
    bytes.push_back(static_cast<char>(slider->getFilterLowRange()));
    bytes.push_back(static_cast<char>(slider->getFilterHighRange()));
    bytes.push_back(static_cast<char>(program.getMidiProgramChange() - 1));
    appendBytes(bytes, kApsProgramSliderPadding, sizeof(kApsProgramSliderPadding));

    for (int i = 0; i < kProgramPadCount; ++i)
    {
        const auto* note = program.getNoteParameters(i + 35);
        bytes.push_back(static_cast<char>(note->getSoundIndex() == -1 ? 0xFF : note->getSoundIndex()));
        bytes.push_back(static_cast<char>(note->getSoundIndex() == -1 ? 0xFF : 0x00));
        bytes.push_back(static_cast<char>(note->getSoundGenerationMode()));
        bytes.push_back(static_cast<char>(note->getVelocityRangeLower()));
        bytes.push_back(static_cast<char>(note->getOptionalNoteA() == 34 ? 0 : note->getOptionalNoteA().get()));
        bytes.push_back(static_cast<char>(note->getVelocityRangeUpper()));
        bytes.push_back(static_cast<char>(note->getOptionalNoteB() == 34 ? 0 : note->getOptionalNoteB().get()));
        bytes.push_back(static_cast<char>(static_cast<int>(note->getVoiceOverlapMode())));
        bytes.push_back(static_cast<char>(note->getMuteAssignA() == 34 ? 0 : note->getMuteAssignA().get()));
        bytes.push_back(static_cast<char>(note->getMuteAssignB() == 34 ? 0 : note->getMuteAssignB().get()));
        appendBytes(bytes, mpc::file::ByteUtil::ushort2bytes(static_cast<uint16_t>(note->getTune())));
        bytes.push_back(static_cast<char>(note->getAttack()));
        bytes.push_back(static_cast<char>(note->getDecay()));
        bytes.push_back(static_cast<char>(note->getDecayMode()));
        bytes.push_back(static_cast<char>(note->getFilterFrequency()));
        bytes.push_back(static_cast<char>(note->getFilterResonance()));
        bytes.push_back(static_cast<char>(note->getFilterAttack()));
        bytes.push_back(static_cast<char>(note->getFilterDecay()));
        bytes.push_back(static_cast<char>(note->getFilterEnvelopeAmount()));
        bytes.push_back(static_cast<char>(note->getVeloToLevel()));
        bytes.push_back(static_cast<char>(note->getVelocityToAttack()));
        bytes.push_back(static_cast<char>(note->getVelocityToStart()));
        bytes.push_back(static_cast<char>(note->getVelocityToFilterFrequency()));
        bytes.push_back(static_cast<char>(note->getSliderParameterNumber()));
        bytes.push_back(static_cast<char>(note->getVelocityToPitch()));
    }

    bytes.push_back(kApsProgramNoteTerminator);

    for (int i = 0; i < kProgramPadCount; ++i)
    {
        const auto stereo = program.getStereoMixerChannel(i);
        const auto indiv = program.getIndivFxMixerChannel(i);
        bytes.push_back(static_cast<char>(indiv->getFxPath()));
        bytes.push_back(static_cast<char>(stereo->getLevel()));
        bytes.push_back(static_cast<char>(stereo->getPanning()));
        bytes.push_back(static_cast<char>(indiv->getVolumeIndividualOut()));
        bytes.push_back(static_cast<char>(indiv->getOutput()));
        bytes.push_back(static_cast<char>(indiv->getFxSendLevel()));
    }

    appendBytes(bytes, kApsProgramMixerPadding, sizeof(kApsProgramMixerPadding));

    for (int i = 0; i < kProgramPadCount; ++i)
    {
        bytes.push_back(static_cast<char>(program.getNoteFromPad(mpc::ProgramPadIndex(i)).get()));
    }

    appendBytes(bytes, mpc::file::ByteUtil::hexToBytes(kPadTailFxBoardSettingsHex));
    return bytes;
}

void showPopup(mpc::Mpc& mpc, const std::string& name, const std::string& ext)
{
    const std::string msg =
        "Loading " +
        mpc::StrUtil::toUpper(mpc::StrUtil::padRight(name, " ", 16) + "." + ext);

    const auto ls = mpc.getLayeredScreen();
    ls->postToUiThread(mpc::utils::Task(
        [ls, msg]
        {
            ls->showPopup(msg);
        }));
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void handleSoundNotFound(mpc::Mpc& mpc, const std::string& soundFileName)
{
    const auto cantFindFileScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::CantFindFileScreen>();

    if (const auto skipAll = cantFindFileScreen->skipAll; !skipAll)
    {
        const auto ls = mpc.getLayeredScreen();
        ls->postToUiThread(mpc::utils::Task(
            [ls, cantFindFileScreen, soundFileName]
            {
                cantFindFileScreen->waitingForUser = true;
                cantFindFileScreen->fileName = soundFileName;
                ls->openScreenById(mpc::lcdgui::ScreenId::CantFindFileScreen);
            }));

        while (cantFindFileScreen->waitingForUser)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void loadSound(mpc::Mpc& mpc,
               const std::string& soundFileName,
               const std::string& ext,
               const std::shared_ptr<mpc::disk::MpcFile>& soundFile,
               const bool headless)
{
    constexpr auto replace = false;
    mpc::disk::SoundLoader soundLoader(mpc, replace);

    if (!headless)
    {
        showPopup(mpc, soundFileName, ext);
    }

    mpc::disk::SoundLoaderResult result;
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

void loadReferencedSounds(mpc2000xl_aps_t& parsed,
                          mpc::Mpc& mpc,
                          const bool headless,
                          std::vector<int>& unavailableSoundIndices,
                          std::map<int, int>& finalSoundIndices)
{
    auto sampler = mpc.getSampler();
    auto disk = mpc.getDisk();

    int skipCount = 0;
    sampler->deleteAllSamples();

    for (int i = 0; i < static_cast<int>(parsed.sound_names()->size()); ++i)
    {
        auto ext = std::string("snd");
        std::shared_ptr<mpc::disk::MpcFile> soundFile;
        const auto soundFileName =
            mpc::StrUtil::replaceAll(stripNullTerminatedField(parsed.sound_names()->at(i)), ' ', "");

        for (auto& f : disk->getAllFiles())
        {
            if (mpc::StrUtil::eqIgnoreCase(
                    mpc::StrUtil::replaceAll(f->getName(), ' ', ""),
                    soundFileName + ".SND"))
            {
                soundFile = f;
                break;
            }
        }

        if (!soundFile || !soundFile->exists())
        {
            for (auto& f : disk->getAllFiles())
            {
                if (mpc::StrUtil::eqIgnoreCase(
                        mpc::StrUtil::replaceAll(f->getName(), ' ', ""),
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
            ++skipCount;

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

void loadPrograms(mpc2000xl_aps_t& parsed,
                  mpc::Mpc& mpc,
                  const std::vector<int>& unavailableSoundIndices,
                  const std::map<int, int>& finalSoundIndices)
{
    auto sampler = mpc.getSampler();

    for (const auto& apsProgram : *parsed.aps_programs())
    {
        if (!apsProgram || apsProgram->body() == nullptr)
        {
            continue;
        }

        const auto programIndex = apsProgram->index();
        auto program = sampler->getProgram(programIndex);
        auto* body = apsProgram->body();
        program->setName(trimNullTerminatedField(body->name()));

        const auto* slider = body->slider();
        const auto pgmSlider = program->getSlider();
        pgmSlider->setAssignNote(mpc::DrumNoteNumber(normalizeOffNote(slider->note())));
        pgmSlider->setTuneLowRange(slider->tune_low());
        pgmSlider->setTuneHighRange(slider->tune_high());
        pgmSlider->setDecayLowRange(slider->decay_low());
        pgmSlider->setDecayHighRange(slider->decay_high());
        pgmSlider->setAttackLowRange(slider->attack_low());
        pgmSlider->setAttackHighRange(slider->attack_high());
        pgmSlider->setFilterLowRange(slider->filter_low());
        pgmSlider->setFilterHighRange(slider->filter_high());
        program->setMidiProgramChange(body->program_change() + 1);

        mpc::performance::UpdateProgramBulk msg;
        auto& perfProgram = msg.program;
        msg.programIndex = mpc::ProgramIndex(programIndex);
        perfProgram.used = true;

        for (int noteIndex = 0; noteIndex < 64; ++noteIndex)
        {
            perfProgram.pads[noteIndex].note =
                mpc::DrumNoteNumber(body->pad_to_note_mapping()->at(noteIndex));

            auto& destNoteParams = perfProgram.noteParameters[noteIndex];
            const auto* sourceMixer = body->pad_mixers()->at(noteIndex).get();
            copyMixer(destNoteParams.stereoMixer, destNoteParams.indivFxMixer, *sourceMixer);

            const auto* srcNoteParams = body->note_parameters()->at(noteIndex).get();
            auto soundIndex = srcNoteParams->sound_index() == 0xFFFF
                ? -1
                : static_cast<int>(srcNoteParams->sound_index());

            if (std::find(begin(unavailableSoundIndices),
                          end(unavailableSoundIndices),
                          soundIndex) != end(unavailableSoundIndices))
            {
                soundIndex = -1;
            }

            if (soundIndex != -1)
            {
                const auto finalSoundIndex = finalSoundIndices.find(soundIndex);
                if (finalSoundIndex != end(finalSoundIndices))
                {
                    soundIndex = finalSoundIndex->second;
                }
            }

            destNoteParams.soundIndex = soundIndex;
            destNoteParams.tune = srcNoteParams->tune();
            destNoteParams.voiceOverlapMode =
                static_cast<mpc::sampler::VoiceOverlapMode>(srcNoteParams->voice_overlap_mode());
            destNoteParams.decayMode = static_cast<int>(srcNoteParams->decay_mode());
            destNoteParams.attack = srcNoteParams->attack();
            destNoteParams.decay = srcNoteParams->decay();
            destNoteParams.filterAttack = srcNoteParams->velocity_envelope_to_filter_attack();
            destNoteParams.filterDecay = srcNoteParams->velocity_envelope_to_filter_decay();
            destNoteParams.filterEnvelopeAmount = srcNoteParams->velocity_envelope_to_filter_amount();
            destNoteParams.filterFrequency = srcNoteParams->cutoff();
            destNoteParams.filterResonance = srcNoteParams->resonance();
            destNoteParams.muteAssignA =
                mpc::DrumNoteNumber(normalizeOffNote(srcNoteParams->mute_assign_1()));
            destNoteParams.muteAssignB =
                mpc::DrumNoteNumber(normalizeOffNote(srcNoteParams->mute_assign_2()));
            destNoteParams.optionalNoteA =
                mpc::DrumNoteNumber(normalizeOffNote(srcNoteParams->also_play_use_note_1()));
            destNoteParams.optionalNoteB =
                mpc::DrumNoteNumber(normalizeOffNote(srcNoteParams->also_play_use_note_2()));
            destNoteParams.sliderParameterNumber =
                static_cast<int>(srcNoteParams->slider_parameter());
            destNoteParams.soundGenerationMode =
                static_cast<int>(srcNoteParams->sound_generation_mode());
            destNoteParams.velocityToStart = srcNoteParams->velocity_to_start();
            destNoteParams.velocityToAttack = srcNoteParams->velocity_to_attack();
            destNoteParams.velocityToFilterFrequency = srcNoteParams->velocity_to_cutoff();
            destNoteParams.velocityToLevel = srcNoteParams->velocity_to_level();
            destNoteParams.velocityRangeLower = srcNoteParams->velocity_range_lower();
            destNoteParams.velocityRangeUpper = srcNoteParams->velocity_range_upper();
            destNoteParams.velocityToPitch = srcNoteParams->velocity_to_pitch();
        }

        perfProgram.slider.attackHighRange = slider->attack_high();
        perfProgram.slider.attackLowRange = slider->attack_low();
        perfProgram.slider.decayHighRange = slider->decay_high();
        perfProgram.slider.decayLowRange = slider->decay_low();
        perfProgram.slider.filterHighRange = slider->filter_high();
        perfProgram.slider.filterLowRange = slider->filter_low();
        perfProgram.slider.assignNote = mpc::DrumNoteNumber(normalizeOffNote(slider->note()));
        perfProgram.slider.tuneHighRange = slider->tune_high();
        perfProgram.slider.tuneLowRange = slider->tune_low();
        perfProgram.midiProgramChange = body->program_change() + 1;

        mpc.getPerformanceManager().lock()->enqueue(
            mpc::performance::PerformanceMessage{std::move(msg)});
    }
}

void loadDrums(mpc2000xl_aps_t& parsed, mpc::Mpc& mpc)
{
    for (int i = 0; i < mpc::Mpc2000XlSpecs::DRUM_BUS_COUNT; ++i)
    {
        auto* drum =
            i == 0 ? parsed.drum1()
                   : i == 1 ? parsed.drum2()
                            : i == 2 ? parsed.drum3()
                                     : parsed.drum4();

        mpc::performance::Drum perfDrum;
        perfDrum.drumBusIndex = mpc::DrumBusIndex(i);

        for (int noteIndex = 0; noteIndex < 64; ++noteIndex)
        {
            const auto* mixer = drum->pad_mixers()->at(noteIndex).get();
            copyMixer(perfDrum.stereoMixers[noteIndex], perfDrum.indivFxMixers[noteIndex], *mixer);
        }

        perfDrum.programIndex = mpc::ProgramIndex(drum->program());
        perfDrum.receivePgmChangeEnabled =
            drum->receive_program_change() == mpc2000xl_aps_t::NO_YES_TRUE;
        perfDrum.receiveMidiVolumeEnabled =
            drum->receive_midi_volume() == mpc2000xl_aps_t::NO_YES_TRUE;

        mpc.getPerformanceManager().lock()->enqueue(
            mpc::performance::UpdateDrumBulk{perfDrum});
    }
}

void loadGlobals(mpc2000xl_aps_t& parsed, mpc::Mpc& mpc)
{
    auto mixerSetupScreen = mpc.screens->get<mpc::lcdgui::ScreenId::MixerSetupScreen>();
    auto* globals = parsed.global_parameters();

    mixerSetupScreen->setRecordMixChangesEnabled(
        globals->record_mix_changes() == mpc2000xl_aps_t::NO_YES_TRUE);
    mixerSetupScreen->setCopyPgmMixToDrumEnabled(
        globals->copy_pgm_mix_to_drum() == mpc2000xl_aps_t::NO_YES_TRUE);
    mixerSetupScreen->setFxDrum(globals->fx_drum());
    mixerSetupScreen->setIndivFxSourceDrum(
        globals->indiv_fx_source() == mpc2000xl_aps_t::MIX_SOURCE_DRUM);
    mixerSetupScreen->setStereoMixSourceDrum(
        globals->stereo_mix_source() == mpc2000xl_aps_t::MIX_SOURCE_DRUM);

    auto drumScreen = mpc.screens->get<mpc::lcdgui::ScreenId::DrumScreen>();
    drumScreen->setPadToIntSound(
        globals->pad_to_internal_sound() == mpc2000xl_aps_t::NO_YES_TRUE);
    mixerSetupScreen->setMasterLevel(globals->master_level());

    auto pgmAssignScreen = mpc.screens->get<mpc::lcdgui::ScreenId::PgmAssignScreen>();
    std::vector<mpc::DrumNoteNumber> masterPadAssign;
    masterPadAssign.reserve(64);
    for (const auto note : *parsed.master_pad_to_note_mapping())
    {
        masterPadAssign.emplace_back(note);
    }
    mpc.getSampler()->setMasterPadAssign(masterPadAssign);
    pgmAssignScreen->setPadAssign(
        globals->pad_assign() == mpc2000xl_aps_t::PAD_ASSIGN_MASTERS);
}

} // namespace

using namespace mpc::file::kaitai;

void ApsIo::loadBytes(mpc::Mpc& mpc,
                      const std::vector<char>& bytes,
                      const bool withoutSounds,
                      bool headless)
{
    const auto canonicalBytes = parseRewrite<mpc2000xl_aps_t>(bytes);
    std::stringstream parseStream(
        std::string(canonicalBytes.begin(), canonicalBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    ::kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    if (parsed.magic() != std::string("\x0a\x05", 2))
    {
        throw std::runtime_error("Invalid APS header");
    }

    std::vector<int> unavailableSoundIndices;
    std::map<int, int> finalSoundIndices;
    if (!withoutSounds)
    {
        loadReferencedSounds(parsed, mpc, headless, unavailableSoundIndices, finalSoundIndices);
    }
    loadPrograms(parsed, mpc, unavailableSoundIndices, finalSoundIndices);
    loadDrums(parsed, mpc);
    loadGlobals(parsed, mpc);
    mpc.getSampler()->setSoundIndex(0);
}

void ApsIo::load(mpc::Mpc& mpc, const std::shared_ptr<mpc::disk::MpcFile>& file, bool headless)
{
    loadBytes(mpc, file->getBytes(), false, headless);
}

std::vector<char> ApsIo::save(mpc::Mpc& mpc, const std::string& apsName)
{
    auto sampler = mpc.getSampler();
    std::vector<char> bytes;

    appendBytes(bytes, std::string("\x0a\x05", 2));
    appendBytes(bytes, mpc::file::ByteUtil::ushort2bytes(static_cast<uint16_t>(sampler->getSoundCount())));

    for (int i = 0; i < sampler->getSoundCount(); ++i)
    {
        appendBytes(bytes, toNullTerminatedAsciiBytes(sampler->getSound(i)->getName()));
    }

    appendBytes(bytes, kApsTopLevelPadAfterSounds, sizeof(kApsTopLevelPadAfterSounds));
    appendBytes(bytes, toNullTerminatedAsciiBytes(apsName));
    appendBytes(bytes, buildGlobalParametersBytes(mpc, sampler->getSoundCount()));

    for (const auto& note : *sampler->getMasterPadAssign())
    {
        bytes.push_back(static_cast<char>(note.get()));
    }

    appendBytes(bytes, kApsDrumPadBeforeFirstDrum, sizeof(kApsDrumPadBeforeFirstDrum));

    for (int i = 0; i < mpc::Mpc2000XlSpecs::DRUM_BUS_COUNT; ++i)
    {
        auto drumBus = mpc.getSequencer()->getDrumBus(mpc::DrumBusIndex(i));
        appendBytes(bytes, buildDrumMixerBytes(drumBus));
        appendBytes(bytes, buildDrumConfigBytes(drumBus, i < 3));
    }

    appendBytes(bytes, kApsAfterDrums, sizeof(kApsAfterDrums));

    for (int i = 0; i < mpc::Mpc2000XlSpecs::MAX_PROGRAM_COUNT; ++i)
    {
        auto program = sampler->getProgram(i);
        if (!program->isUsed())
        {
            continue;
        }

        appendBytes(bytes, buildProgramBytes(*program, i));
    }

    appendBytes(bytes, kApsEndMarker, sizeof(kApsEndMarker));
    return parseRewrite<mpc2000xl_aps_t>(bytes);
}
