#include "file/kaitai/PgmIo.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "disk/MpcFile.hpp"
#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"
#include "file/ByteUtil.hpp"
#include "file/kaitai/KaitaiIoUtil.hpp"
#include "file/kaitai/generated/mpc2000xl_pgm.h"
#include "performance/PerformanceManager.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/Pad.hpp"
#include "sampler/PgmSlider.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"

#include <array>
#include <set>
#include <sstream>

#include <kaitai/kaitaistream.h>

namespace {

constexpr uint8_t kUnassignedSoundIndex = 0xFF;
constexpr uint8_t kRawMuteAssignOff = 0;
constexpr int kNormalizedMuteAssignOff = 34;
constexpr int kProgramNameLength = 16;
constexpr int kSoundNameLength = 16;
constexpr int kProgramPadCount = 64;
constexpr int kDefaultSliderUnknown0 = 35;
constexpr int kDefaultSliderUnknown1 = 64;
constexpr int kDefaultSliderUnknown2 = 0;
constexpr int kDefaultSliderUnknown3 = 25;
constexpr int kDefaultSliderUnknown4 = 0;
constexpr int kDefaultNoteParametersTerminator = 6;
constexpr int kDefaultMixerUnknown0 = 0;
constexpr int kDefaultMixerUnknown1 = 64;
constexpr int kDefaultMixerUnknown2 = 0;
constexpr char kPadTailFxBoardSettingsHex[] =
    "02004800D0070000630114081DFC323302323C08050A1414320000020F19000541141E0105"
    "0000056300F4FF0C0000000000000002004F014F0100424F0100424F01004232006328003C"
    "0000D0070000630114081DFC323302323C08050A1414320000020F19000541141E01050000"
    "056300F4FF0C0000000000000002004F014F0100424F0100424F01004232006328003C0000"
    "04000C000000320023003E335A3214000000320023003E335A3214000000320023003E335A"
    "3214000000320023003E335A321400";

int normalizeMuteAssign(const uint8_t rawValue)
{
    return rawValue == kRawMuteAssignOff ? kNormalizedMuteAssignOff : rawValue;
}

std::string stripNullTerminatedField(const std::string& raw)
{
    const auto terminator = raw.find('\0');
    return terminator == std::string::npos ? raw : raw.substr(0, terminator);
}

std::string toNullTerminatedAsciiField(const std::string& value, const int visibleLength)
{
    auto field = mpc::StrUtil::padRight(value.substr(0, visibleLength), " ", visibleLength);
    field.push_back('\0');
    return field;
}

void setSlider(const mpc2000xl_pgm_t& parsed,
               const std::shared_ptr<mpc::sampler::Program>& program)
{
    const auto* slider = parsed.slider();
    const auto note = slider->note() == 0
        ? mpc::NoDrumNoteAssigned
        : mpc::DrumNoteNumber(slider->note());
    const auto pgmSlider = program->getSlider();
    pgmSlider->setAssignNote(note);
    pgmSlider->setAttackHighRange(slider->attack_high());
    pgmSlider->setAttackLowRange(slider->attack_low());
    pgmSlider->setDecayHighRange(slider->decay_high());
    pgmSlider->setDecayLowRange(slider->decay_low());
    pgmSlider->setFilterHighRange(slider->filter_high());
    pgmSlider->setFilterLowRange(slider->filter_low());
    pgmSlider->setTuneHighRange(slider->tune_high());
    pgmSlider->setTuneLowRange(slider->tune_low());
}

void setNoteParameters(mpc::Mpc& mpc,
                       const mpc2000xl_pgm_t& parsed,
                       const std::shared_ptr<mpc::sampler::Program>& program)
{
    std::array<mpc::performance::NoteParameters, mpc::Mpc2000XlSpecs::PROGRAM_PAD_COUNT>
        allPerfNoteParams;

    for (int programPadIndex = 0;
         programPadIndex < mpc::Mpc2000XlSpecs::PROGRAM_PAD_COUNT;
         ++programPadIndex)
    {
        const auto padNote = parsed.pad_to_note_mapping()->at(programPadIndex);
        const auto note = padNote == -1 ? mpc::NoDrumNoteAssigned : padNote;
        program->getPad(programPadIndex)->setNote(mpc::DrumNoteNumber(note));

        const auto* parsedNote = parsed.note_parameters()->at(programPadIndex).get();
        auto& perfNoteParams = allPerfNoteParams[programPadIndex];

        perfNoteParams.attack = parsedNote->attack();
        perfNoteParams.decay = parsedNote->decay();
        perfNoteParams.decayMode = static_cast<int>(parsedNote->decay_mode());
        perfNoteParams.filterAttack = parsedNote->velocity_envelope_to_filter_attack();
        perfNoteParams.filterDecay = parsedNote->velocity_envelope_to_filter_decay();
        perfNoteParams.filterEnvelopeAmount = parsedNote->velocity_envelope_to_filter_amount();
        perfNoteParams.filterFrequency = parsedNote->cutoff();
        perfNoteParams.filterResonance = parsedNote->resonance();
        perfNoteParams.muteAssignA = mpc::DrumNoteNumber(normalizeMuteAssign(parsedNote->mute_assign_1()));
        perfNoteParams.muteAssignB = mpc::DrumNoteNumber(normalizeMuteAssign(parsedNote->mute_assign_2()));
        perfNoteParams.optionalNoteA = mpc::DrumNoteNumber(parsedNote->also_play_use_note_1());
        perfNoteParams.optionalNoteB = mpc::DrumNoteNumber(parsedNote->also_play_use_note_2());
        perfNoteParams.soundIndex = parsedNote->sound_index() == kUnassignedSoundIndex
            ? -1
            : parsedNote->sound_index();
        perfNoteParams.sliderParameterNumber = static_cast<int>(parsedNote->slider_parameter());
        perfNoteParams.soundGenerationMode = static_cast<int>(parsedNote->sound_generation_mode());
        perfNoteParams.tune = parsedNote->tune();
        perfNoteParams.velocityRangeLower = parsedNote->velocity_range_lower();
        perfNoteParams.velocityRangeUpper = parsedNote->velocity_range_upper();
        perfNoteParams.velocityToAttack = parsedNote->velocity_to_attack();
        perfNoteParams.velocityToFilterFrequency = parsedNote->velocity_to_cutoff();
        perfNoteParams.velocityToLevel = parsedNote->velocity_to_level();
        perfNoteParams.velocityToPitch = parsedNote->velocity_to_pitch();
        perfNoteParams.velocityToStart = parsedNote->velocity_to_start();
        perfNoteParams.voiceOverlapMode = static_cast<mpc::sampler::VoiceOverlapMode>(parsedNote->voice_overlap_mode());
    }

    mpc::performance::UpdateAllNoteParametersBulk msg{program->getProgramIndex(),
                                                      allPerfNoteParams};
    mpc.getPerformanceManager().lock()->enqueue(msg);
}

void setMixer(const mpc2000xl_pgm_t& parsed,
              const std::shared_ptr<mpc::sampler::Program>& program)
{
    for (int i = 0; i < 64; ++i)
    {
        const auto noteParameters = program->getNoteParameters(i + 35);
        const auto stereoMixer = noteParameters->getStereoMixer();
        const auto indivFxMixer = noteParameters->getIndivFxMixer();
        const auto* parsedMixer = parsed.pad_mixers()->at(i).get();

        stereoMixer->setLevel(mpc::DrumMixerLevel(parsedMixer->volume()));
        stereoMixer->setPanning(mpc::DrumMixerPanning(parsedMixer->pan()));
        indivFxMixer->setVolumeIndividualOut(
            mpc::DrumMixerLevel(parsedMixer->volume_individual()));
        indivFxMixer->setOutput(
            mpc::DrumMixerIndividualOutput(parsedMixer->output()));
        indivFxMixer->setFxPath(
            mpc::DrumMixerIndividualFxPath(parsedMixer->fx_output()));
        indivFxMixer->setFxSendLevel(
            mpc::DrumMixerLevel(parsedMixer->effects_send_level()));
    }
}

std::vector<std::string> collectSavedSoundNames(
    mpc::sampler::Program& program,
    const std::shared_ptr<mpc::sampler::Sampler>& sampler)
{
    std::vector<int> soundIndices;
    for (auto* noteParameters : program.getNotesParameters())
    {
        if (noteParameters->getSoundIndex() != -1)
        {
            soundIndices.push_back(noteParameters->getSoundIndex());
        }
    }

    std::set<std::string> uniqueSoundNames;
    std::vector<std::string> finalNames;
    for (const auto soundIndex : soundIndices)
    {
        const auto soundName = sampler->getSoundName(soundIndex);
        if (uniqueSoundNames.emplace(soundName).second)
        {
            finalNames.push_back(soundName);
        }
    }
    return finalNames;
}

std::vector<int> buildSoundIndexConversionTable(
    const std::vector<std::string>& finalSoundNames,
    const std::shared_ptr<mpc::sampler::Sampler>& sampler)
{
    std::vector<int> conversionTable;
    conversionTable.reserve(sampler->getSoundCount());

    for (int soundIndex = 0; soundIndex < sampler->getSoundCount(); ++soundIndex)
    {
        const auto soundName = sampler->getSoundName(soundIndex);
        int mappedIndex = -1;
        for (int i = 0; i < static_cast<int>(finalSoundNames.size()); ++i)
        {
            if (finalSoundNames[i] == soundName)
            {
                mappedIndex = i;
                break;
            }
        }
        conversionTable.push_back(mappedIndex);
    }

    return conversionTable;
}

void populateSavedSlider(mpc2000xl_pgm_t& parsed)
{
    parsed.set__unnamed7(std::string{
        static_cast<char>(kDefaultSliderUnknown0),
        static_cast<char>(kDefaultSliderUnknown1),
        static_cast<char>(kDefaultSliderUnknown2),
        static_cast<char>(kDefaultSliderUnknown3),
        static_cast<char>(kDefaultSliderUnknown4)
    });
}

void populateSavedNoteParameters(
    mpc2000xl_pgm_t& parsed,
    mpc::sampler::Program& program,
    const std::vector<int>& soundIndexConversionTable)
{
    auto notes = std::make_unique<std::vector<std::unique_ptr<mpc2000xl_pgm_t::note_t>>>();
    notes->reserve(kProgramPadCount);

    for (int i = 0; i < kProgramPadCount; ++i)
    {
        auto note = std::make_unique<mpc2000xl_pgm_t::note_t>(nullptr, &parsed, &parsed);
        const auto* src = program.getNoteParameters(i + 35);

        if (src->getSoundIndex() == -1)
        {
            note->set_sound_index(kUnassignedSoundIndex);
        }
        else
        {
            note->set_sound_index(static_cast<uint8_t>(soundIndexConversionTable.at(src->getSoundIndex())));
        }

        note->set_sound_generation_mode(static_cast<mpc2000xl_pgm_t::sound_generation_mode_t>(src->getSoundGenerationMode()));
        note->set_velocity_range_lower(src->getVelocityRangeLower());
        note->set_also_play_use_note_1(src->getOptionalNoteA());
        note->set_velocity_range_upper(src->getVelocityRangeUpper());
        note->set_also_play_use_note_2(src->getOptionalNoteB());
        note->set_voice_overlap_mode(static_cast<mpc2000xl_pgm_t::voice_overlap_mode_t>(static_cast<int>(src->getVoiceOverlapMode())));
        note->set_mute_assign_1(src->getMuteAssignA() == 34 ? 0 : src->getMuteAssignA());
        note->set_mute_assign_2(src->getMuteAssignB() == 34 ? 0 : src->getMuteAssignB());
        note->set_tune(static_cast<int16_t>(src->getTune()));
        note->set_attack(src->getAttack());
        note->set_decay(src->getDecay());
        note->set_decay_mode(static_cast<mpc2000xl_pgm_t::decay_mode_t>(src->getDecayMode()));
        note->set_cutoff(src->getFilterFrequency());
        note->set_resonance(src->getFilterResonance());
        note->set_velocity_envelope_to_filter_attack(src->getFilterAttack());
        note->set_velocity_envelope_to_filter_decay(src->getFilterDecay());
        note->set_velocity_envelope_to_filter_amount(src->getFilterEnvelopeAmount());
        note->set_velocity_to_level(src->getVeloToLevel());
        note->set_velocity_to_attack(src->getVelocityToAttack());
        note->set_velocity_to_start(src->getVelocityToStart());
        note->set_velocity_to_cutoff(src->getVelocityToFilterFrequency());
        note->set_slider_parameter(static_cast<mpc2000xl_pgm_t::slider_parameter_t>(src->getSliderParameterNumber()));
        note->set_velocity_to_pitch(src->getVelocityToPitch());

        notes->push_back(std::move(note));
    }

    parsed.set_note_parameters(std::move(notes));
    parsed.set__unnamed9(std::string{static_cast<char>(kDefaultNoteParametersTerminator)});
}

void populateSavedMixers(mpc2000xl_pgm_t& parsed, mpc::sampler::Program& program)
{
    auto mixers = std::make_unique<std::vector<std::unique_ptr<mpc2000xl_pgm_t::pad_mixer_t>>>();
    mixers->reserve(kProgramPadCount);

    for (int i = 0; i < kProgramPadCount; ++i)
    {
        auto mixer = std::make_unique<mpc2000xl_pgm_t::pad_mixer_t>(nullptr, &parsed, &parsed);
        const auto* src = program.getNoteParameters(i + 35);
        const auto stereo = src->getStereoMixer();
        const auto indiv = src->getIndivFxMixer();

        mixer->set_fx_output(static_cast<mpc2000xl_pgm_t::fx_output_t>(static_cast<int>(indiv->getFxPath())));
        mixer->set_volume(stereo->getLevel());
        mixer->set_pan(stereo->getPanning());
        mixer->set_volume_individual(indiv->getVolumeIndividualOut());
        mixer->set_output(indiv->getOutput());
        mixer->set_effects_send_level(indiv->getFxSendLevel());
        mixers->push_back(std::move(mixer));
    }

    parsed.set_pad_mixers(std::move(mixers));
    parsed.set__unnamed11(std::string{
        static_cast<char>(kDefaultMixerUnknown0),
        static_cast<char>(kDefaultMixerUnknown1),
        static_cast<char>(kDefaultMixerUnknown2)
    });
}

std::string fxBoardSettingsBytes()
{
    const auto bytes = mpc::file::ByteUtil::hexToBytes(kPadTailFxBoardSettingsHex);
    return std::string(bytes.begin(), bytes.end());
}

} // namespace

using namespace mpc::file::kaitai;

program_or_error PgmIo::loadProgram(mpc::Mpc &mpc,
                                    const std::shared_ptr<mpc::disk::MpcFile> &file,
                                    const std::shared_ptr<mpc::sampler::Program> &program,
                                    std::vector<std::string> &soundNames)
{
    const auto fileBytes = file->getBytes();
    const auto canonicalBytes = parseRewrite<mpc2000xl_pgm_t>(fileBytes);
    std::stringstream parseStream(
        std::string(canonicalBytes.begin(), canonicalBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    ::kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();

    if (parsed.magic() != std::string("\x07\x04", 2))
    {
        return tl::make_unexpected(mpc::disk::mpc_io_error_msg{"PGM first 2 bytes are incorrect"});
    }

    soundNames.clear();
    soundNames.reserve(parsed.sound_count());
    for (int i = 0; i < parsed.sound_count(); ++i)
    {
        soundNames.push_back(stripNullTerminatedField(parsed.sound_names()->at(i)));
    }

    const std::string programNameInData = mpc::StrUtil::trim(stripNullTerminatedField(parsed.name()));
    if (mpc::StrUtil::eqIgnoreCase(programNameInData, file->getNameWithoutExtension()))
    {
        program->setName(programNameInData);
    }
    else
    {
        program->setName(file->getNameWithoutExtension());
    }

    setNoteParameters(mpc, parsed, program);
    setMixer(parsed, program);
    setSlider(parsed, program);
    program->setMidiProgramChange(parsed.program_change() + 1);

    return program;
}

std::vector<char> PgmIo::saveProgram(mpc::sampler::Program &program,
                                     const std::weak_ptr<mpc::sampler::Sampler> &sampler)
{
    const auto lockedSampler = sampler.lock();
    if (!lockedSampler)
    {
        return {};
    }

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    ::kaitai::kstream writeIo(&writeStream);
    mpc2000xl_pgm_t parsed(&writeIo);

    const auto finalSoundNames = collectSavedSoundNames(program, lockedSampler);
    const auto soundIndexConversionTable =
        buildSoundIndexConversionTable(finalSoundNames, lockedSampler);

    auto soundNames = std::make_unique<std::vector<std::string>>();
    soundNames->reserve(finalSoundNames.size());
    for (const auto& soundName : finalSoundNames)
    {
        soundNames->push_back(toNullTerminatedAsciiField(soundName, kSoundNameLength));
    }

    auto slider = std::make_unique<mpc2000xl_pgm_t::slider_t>(nullptr, &parsed, &parsed);
    const auto* srcSlider = program.getSlider();
    slider->set_note(srcSlider->getNote() == 34 ? 0 : srcSlider->getNote());
    slider->set_tune_low(srcSlider->getTuneLowRange());
    slider->set_tune_high(srcSlider->getTuneHighRange());
    slider->set_decay_low(srcSlider->getDecayLowRange());
    slider->set_decay_high(srcSlider->getDecayHighRange());
    slider->set_attack_low(srcSlider->getAttackLowRange());
    slider->set_attack_high(srcSlider->getAttackHighRange());
    slider->set_filter_low(srcSlider->getFilterLowRange());
    slider->set_filter_high(srcSlider->getFilterHighRange());

    parsed.set_magic(std::string("\x07\x04", 2));
    parsed.set_sound_count(static_cast<uint16_t>(finalSoundNames.size()));
    parsed.set_sound_names(std::move(soundNames));
    parsed.set__unnamed3(std::string{30, 0});
    parsed.set_name(toNullTerminatedAsciiField(program.getName(), kProgramNameLength));
    parsed.set_slider(std::move(slider));
    parsed.set_program_change(static_cast<uint8_t>(program.getMidiProgramChange() - 1));
    populateSavedSlider(parsed);
    populateSavedNoteParameters(parsed, program, soundIndexConversionTable);
    populateSavedMixers(parsed, program);

    auto padMapping = std::make_unique<std::vector<int8_t>>();
    padMapping->reserve(kProgramPadCount);
    for (int i = 0; i < kProgramPadCount; ++i)
    {
        const auto note = program.getPad(i)->getNote();
        padMapping->push_back(static_cast<int8_t>(note == 34 ? 0 : note.get()));
    }
    parsed.set_pad_to_note_mapping(std::move(padMapping));
    parsed.set__unnamed13(fxBoardSettingsBytes());

    parsed._check();
    parsed._write();

    const auto bytes = writeStream.str();
    return std::vector<char>(bytes.begin(), bytes.end());
}
