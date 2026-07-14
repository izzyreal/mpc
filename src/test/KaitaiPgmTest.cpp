#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "IntTypes.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/ProgramLoader.hpp"
#include "engine/IndivFxMixer.hpp"
#include "engine/EngineHost.hpp"
#include "engine/StereoMixer.hpp"
#include "file/kaitai/PgmIo.hpp"
#include "file/kaitai/generated/mpc2000xl_pgm.h"
#include "file/kaitai/generated/mpc3000_pgm_v3.h"
#include "performance/PerformanceManager.hpp"
#include "sampler/Pad.hpp"
#include "sampler/PgmSlider.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/SoundGenerationMode.hpp"
#include "sampler/VoiceOverlapMode.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

CMRC_DECLARE(mpctest);

namespace {

std::vector<char> rewriteProgram1WithMutations(
    const std::function<void(mpc2000xl_pgm_t&)>& mutate)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();

    mutate(parsed);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    return std::vector<char>(rewrittenBytes.begin(), rewrittenBytes.end());
}

std::vector<char> rewriteRealMpc3000ProgramWithMutations(
    const std::string& resourcePath,
    const std::function<void(mpc3000_pgm_v3_t&)>& mutate)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open(resourcePath);
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_pgm_v3_t parsed(&parseIo);
    parsed._read();

    mutate(parsed);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    return std::vector<char>(rewrittenBytes.begin(), rewrittenBytes.end());
}

void applyBroadProgramMutation(mpc2000xl_pgm_t& parsed)
{
    auto* note35 = parsed.note_parameters()->at(0).get();
    note35->set_sound_index(255);
    note35->set_sound_generation_mode(mpc2000xl_pgm_t::SOUND_GENERATION_MODE_DCY_SW);
    note35->set_velocity_range_lower(1);
    note35->set_velocity_range_upper(127);
    note35->set_also_play_use_note_1(41);
    note35->set_also_play_use_note_2(42);
    note35->set_voice_overlap_mode(mpc2000xl_pgm_t::VOICE_OVERLAP_MODE_NOTE_OFF);
    note35->set_mute_assign_1(43);
    note35->set_mute_assign_2(44);
    note35->set_tune(-120);
    note35->set_attack(99);
    note35->set_decay(98);
    note35->set_decay_mode(mpc2000xl_pgm_t::DECAY_MODE_START);
    note35->set_cutoff(17);
    note35->set_resonance(18);
    note35->set_velocity_envelope_to_filter_attack(19);
    note35->set_velocity_envelope_to_filter_decay(20);
    note35->set_velocity_envelope_to_filter_amount(21);
    note35->set_velocity_to_level(22);
    note35->set_velocity_to_attack(23);
    note35->set_velocity_to_start(24);
    note35->set_velocity_to_cutoff(25);
    note35->set_slider_parameter(mpc2000xl_pgm_t::SLIDER_PARAMETER_FILTER);
    note35->set_velocity_to_pitch(26);

    auto* mixer35 = parsed.pad_mixers()->at(0).get();
    mixer35->set_fx_output(mpc2000xl_pgm_t::FX_OUTPUT_R2);
    mixer35->set_volume(12);
    mixer35->set_pan(13);
    mixer35->set_volume_individual(14);
    mixer35->set_output(2);
    mixer35->set_effects_send_level(15);

    parsed.slider()->set_note(52);
    parsed.slider()->set_tune_low(-119);
    parsed.slider()->set_tune_high(118);
    parsed.slider()->set_decay_low(11);
    parsed.slider()->set_decay_high(12);
    parsed.slider()->set_attack_low(13);
    parsed.slider()->set_attack_high(14);
    parsed.slider()->set_filter_low(-15);
    parsed.slider()->set_filter_high(16);

    parsed.set_program_change(126);
    parsed.pad_to_note_mapping()->at(0) = 50;
}

std::string mpc3000SoundNameField(const std::string& name)
{
    return name.substr(0, 16);
}

void requireBroadMutatedProgramState(
    const std::shared_ptr<mpc::sampler::Program>& program,
    const std::vector<std::string>& soundNames)
{
    REQUIRE(program != nullptr);
    REQUIRE(soundNames.size() == 2U);
    REQUIRE(soundNames[0] == std::string("sound1") + std::string(10, ' '));
    REQUIRE(soundNames[1] == std::string("sound2") + std::string(10, ' '));

    REQUIRE(program->getPad(0)->getNote() == 50);
    REQUIRE(program->getMidiProgramChange() == 127);

    const auto note35 = program->getNoteParameters(35);
    REQUIRE(note35->getSoundIndex() == -1);
    REQUIRE(note35->getSoundGenerationMode() ==
            mpc::sampler::SoundGenerationMode::DecaySwitch);
    REQUIRE(note35->getVelocityRangeLower() == 1);
    REQUIRE(note35->getVelocityRangeUpper() == 127);
    REQUIRE(note35->getOptionalNoteA() == 41);
    REQUIRE(note35->getOptionalNoteB() == 42);
    REQUIRE(note35->getMuteAssignA() == 43);
    REQUIRE(note35->getMuteAssignB() == 44);
    REQUIRE(note35->getTune() == -120);
    REQUIRE(note35->getAttack() == 99);
    REQUIRE(note35->getDecay() == 98);
    REQUIRE(note35->getDecayMode() == 1);
    REQUIRE(note35->getFilterFrequency() == 17);
    REQUIRE(note35->getFilterResonance() == 18);
    REQUIRE(note35->getFilterAttack() == 19);
    REQUIRE(note35->getFilterDecay() == 20);
    REQUIRE(note35->getFilterEnvelopeAmount() == 21);
    REQUIRE(note35->getVeloToLevel() == 22);
    REQUIRE(note35->getVelocityToAttack() == 23);
    REQUIRE(note35->getVelocityToStart() == 24);
    REQUIRE(note35->getVelocityToFilterFrequency() == 25);
    REQUIRE(note35->getSliderParameterNumber() == 3);
    REQUIRE(note35->getVelocityToPitch() == 26);
    REQUIRE(note35->getVoiceOverlapMode() == mpc::sampler::VoiceOverlapMode::NOTE_OFF);

    const auto stereoMixer = note35->getStereoMixer();
    const auto indivFxMixer = note35->getIndivFxMixer();
    REQUIRE(stereoMixer->getLevel() == 12);
    REQUIRE(stereoMixer->getPanning() == 13);
    REQUIRE(indivFxMixer->getVolumeIndividualOut() == 14);
    REQUIRE(indivFxMixer->getOutput() == 2);
    REQUIRE(indivFxMixer->getFxPath() == 4);
    REQUIRE(indivFxMixer->getFxSendLevel() == 15);

    const auto slider = program->getSlider();
    REQUIRE(slider->getNote() == 52);
    REQUIRE(slider->getTuneLowRange() == -119);
    REQUIRE(slider->getTuneHighRange() == 118);
    REQUIRE(slider->getDecayLowRange() == 11);
    REQUIRE(slider->getDecayHighRange() == 12);
    REQUIRE(slider->getAttackLowRange() == 13);
    REQUIRE(slider->getAttackHighRange() == 14);
    REQUIRE(slider->getFilterLowRange() == -15);
    REQUIRE(slider->getFilterHighRange() == 16);
}

void prepareProgramLoadingResources(mpc::Mpc& mpc)
{
    auto disk = mpc.getDisk();
    auto fs = cmrc::mpctest::get_filesystem();

    for (auto&& entry : fs.iterate_directory("test/ProgramLoading")) {
        disk->newFolder(entry.filename());
        disk->initFiles();
        disk->moveForward(entry.filename());
        disk->initFiles();

        for (auto&& entry2 : fs.iterate_directory("test/ProgramLoading/" + entry.filename())) {
            auto file = fs.open("test/ProgramLoading/" + entry.filename() + "/" + entry2.filename());
            std::vector<char> data(file.begin(), file.end());
            auto newFile = disk->newFile(entry2.filename());
            newFile->setFileData(data);
        }

        disk->moveBack();
        disk->initFiles();
    }
}

std::shared_ptr<mpc::sampler::Program> loadProgram1(mpc::Mpc& mpc)
{
    prepareProgramLoadingResources(mpc);

    auto disk = mpc.getDisk();
    disk->moveForward("program1");
    disk->initFiles();
    auto pgmFile = disk->getFile("PROGRAM1.PGM");

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto program = sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    (void)mpc::disk::ProgramLoader::loadProgram(mpc, pgmFile, program, 0);
    mpc.getEngineHost()->prepareProcessBlock(512);
    return program;
}

std::shared_ptr<mpc::sampler::Program> loadProgram1FromBytes(
    mpc::Mpc& mpc,
    const std::vector<char>& programBytes)
{
    prepareProgramLoadingResources(mpc);

    auto disk = mpc.getDisk();
    disk->moveForward("program1");
    disk->initFiles();
    auto pgmFile = disk->getFile("PROGRAM1.PGM");
    auto mutableBytes = programBytes;
    pgmFile->setFileData(mutableBytes);

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto program = sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    (void)mpc::disk::ProgramLoader::loadProgram(mpc, pgmFile, program, 0);
    mpc.getEngineHost()->prepareProcessBlock(512);
    return program;
}

std::shared_ptr<mpc::sampler::Program> loadProgram1FromBytesWithoutRender(
    mpc::Mpc& mpc,
    const std::vector<char>& programBytes)
{
    prepareProgramLoadingResources(mpc);

    auto disk = mpc.getDisk();
    disk->moveForward("program1");
    disk->initFiles();
    auto pgmFile = disk->getFile("PROGRAM1.PGM");
    auto mutableBytes = programBytes;
    pgmFile->setFileData(mutableBytes);

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto program = sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    (void)mpc::disk::ProgramLoader::loadProgram(mpc, pgmFile, program, 0);
    mpc.getPerformanceManager().lock()->drainQueue();
    return program;
}

std::shared_ptr<mpc::disk::MpcFile> loadProgram1FileFromBytes(
    mpc::Mpc& mpc,
    const std::vector<char>& programBytes)
{
    prepareProgramLoadingResources(mpc);

    auto disk = mpc.getDisk();
    disk->moveForward("program1");
    disk->initFiles();
    auto pgmFile = disk->getFile("PROGRAM1.PGM");
    auto mutableBytes = programBytes;
    pgmFile->setFileData(mutableBytes);
    return pgmFile;
}

std::shared_ptr<mpc::sampler::Program> loadProgram1ViaPgmIoFromBytes(
    mpc::Mpc& mpc,
    const std::vector<char>& programBytes,
    std::vector<std::string>& soundNames)
{
    auto pgmFile = loadProgram1FileFromBytes(mpc, programBytes);

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto program = sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    auto result = mpc::file::kaitai::PgmIo::loadProgram(mpc, pgmFile, program, soundNames);
    REQUIRE(result);
    mpc.getPerformanceManager().lock()->drainQueue();
    return program;
}

std::shared_ptr<mpc::sampler::Program> loadProgramViaPgmIoFromBytes(
    mpc::Mpc& mpc,
    const std::string& filename,
    const std::vector<char>& programBytes,
    std::vector<std::string>& soundNames)
{
    auto disk = mpc.getDisk();
    auto file = disk->newFile(filename);
    auto mutableBytes = programBytes;
    file->setFileData(mutableBytes);
    disk->initFiles();

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto program = sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    auto result = mpc::file::kaitai::PgmIo::loadProgram(mpc, file, program, soundNames);
    REQUIRE(result);
    mpc.getPerformanceManager().lock()->drainQueue();
    return program;
}

void requireProgram1LoadedState(mpc::Mpc& mpc,
                                const std::shared_ptr<mpc::sampler::Program>& program)
{
    REQUIRE(program != nullptr);
    REQUIRE(program->getName() == "PROGRAM1");

    auto sampler = mpc.getSampler();
    REQUIRE(sampler->getSoundCount() == 2);
    REQUIRE(sampler->getSoundName(0) == "sound1");
    REQUIRE(sampler->getSoundName(1) == "sound2");
    REQUIRE(sampler->getSound(1)->getEnd() == 1000);

    const auto note35 = program->getNoteParameters(35);
    const auto note36 = program->getNoteParameters(36);
    const auto note37 = program->getNoteParameters(37);

    REQUIRE(note35->getSoundIndex() == 0);
    REQUIRE(note36->getSoundIndex() == 1);
    REQUIRE(note37->getSoundIndex() == -1);

    REQUIRE(note35->getSoundGenerationMode() ==
            mpc::sampler::SoundGenerationMode::Normal);
    REQUIRE(note35->getVelocityRangeLower() == 44);
    REQUIRE(note35->getVelocityRangeUpper() == 88);
    REQUIRE(note35->getTune() == 0);
    REQUIRE(note35->getAttack() == 0);
    REQUIRE(note35->getDecay() == 5);
    REQUIRE(note35->getDecayMode() == 0);
    REQUIRE(note35->getFilterFrequency() == 100);
    REQUIRE(note35->getFilterResonance() == 0);
    REQUIRE(note35->getFilterAttack() == 0);
    REQUIRE(note35->getFilterDecay() == 0);
    REQUIRE(note35->getFilterEnvelopeAmount() == 0);
    REQUIRE(note35->getVeloToLevel() == 100);
    REQUIRE(note35->getVelocityToAttack() == 0);
    REQUIRE(note35->getVelocityToStart() == 0);
    REQUIRE(note35->getVelocityToFilterFrequency() == 0);
    REQUIRE(note35->getSliderParameterNumber() == 0);
    REQUIRE(note35->getVelocityToPitch() == 0);
    REQUIRE(note35->getVoiceOverlapMode() == mpc::sampler::VoiceOverlapMode::POLY);
    REQUIRE(note35->getOptionalNoteA() == mpc::NoDrumNoteAssigned);
    REQUIRE(note35->getOptionalNoteB() == mpc::NoDrumNoteAssigned);
    REQUIRE(note35->getMuteAssignA() == mpc::NoDrumNoteAssigned);
    REQUIRE(note35->getMuteAssignB() == mpc::NoDrumNoteAssigned);

    const auto stereoMixer = note35->getStereoMixer();
    const auto indivFxMixer = note35->getIndivFxMixer();
    REQUIRE(stereoMixer->getLevel() == mpc::MaxDrumMixerLevel);
    REQUIRE(stereoMixer->getPanning() == mpc::PanningCenter);
    REQUIRE(indivFxMixer->getVolumeIndividualOut() == mpc::MaxDrumMixerLevel);
    REQUIRE(indivFxMixer->getOutput() == mpc::MinDrumMixerIndividualOut);
    REQUIRE(indivFxMixer->getFxPath() == mpc::MinDrumMixerFxPath);
    REQUIRE(indivFxMixer->getFxSendLevel() == mpc::MinDrumMixerLevel);

    const auto slider = program->getSlider();
    REQUIRE(slider->getNote() == mpc::NoDrumNoteAssigned);
    REQUIRE(slider->getTuneLowRange() == -120);
    REQUIRE(slider->getTuneHighRange() == 120);
    REQUIRE(slider->getDecayLowRange() == 12);
    REQUIRE(slider->getDecayHighRange() == 45);
    REQUIRE(slider->getAttackLowRange() == 0);
    REQUIRE(slider->getAttackHighRange() == 20);
    REQUIRE(slider->getFilterLowRange() == -50);
    REQUIRE(slider->getFilterHighRange() == 50);
    REQUIRE(slider->getControlChange() == 0);
    REQUIRE(slider->getParameter() == mpc::NoteVariationTypeTune);
    REQUIRE(program->getMidiProgramChange() == 1);
}

}

TEST_CASE("Kaitai MPC2000 PGM parses and rewrites PROGRAM1", "[kaitai-pgm]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x07\x04", 2));
    REQUIRE(parsed.sound_count() == 2);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 2U);
    REQUIRE(parsed.sound_names()->at(0).size() == 17U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 6) == "sound1");
    REQUIRE(parsed.sound_names()->at(0).back() == '\0');
    REQUIRE(parsed.sound_names()->at(1).size() == 17U);
    REQUIRE(parsed.sound_names()->at(1).substr(0, 6) == "sound2");
    REQUIRE(parsed.sound_names()->at(1).back() == '\0');
    REQUIRE(parsed.name().size() == 17U);
    REQUIRE(parsed.name().substr(0, 16) == "VMPCTESTPROGRAM1");
    REQUIRE(parsed.name().back() == '\0');
    REQUIRE(parsed.note_parameters() != nullptr);
    REQUIRE(parsed.note_parameters()->size() == 64U);
    REQUIRE(parsed.note_parameters()->at(0)->sound_index() == 0);
    REQUIRE(parsed.note_parameters()->at(1)->sound_index() == 1);
    // Hardware-backed comparison shows this standalone byte stores MIDI
    // program change as a zero-based value.
    REQUIRE(parsed.program_change() == 0);
    REQUIRE(parsed.pad_to_note_mapping() != nullptr);
    REQUIRE(parsed.pad_to_note_mapping()->size() == 64U);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    REQUIRE(writeStream.str() == originalBytes);
}

TEST_CASE("Kaitai MPC2000 PGM parses and rewrites real 2KXL NEWPGM-A", "[kaitai-pgm][real-2kxl]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc2000xl/Pgm/NEWPGM-A.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x07\x04", 2));
    REQUIRE(parsed.sound_count() == 2);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 2U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 2) == "ML");
    REQUIRE(parsed.sound_names()->at(0).back() == '\0');
    REQUIRE(parsed.sound_names()->at(1).substr(0, 1) == "M");
    REQUIRE(parsed.sound_names()->at(1).back() == '\0');
    REQUIRE(parsed.name().size() == 17U);
    REQUIRE(parsed.name().substr(0, 8) == "NewPgm-A");
    REQUIRE(parsed.name().back() == '\0');
    REQUIRE(parsed.note_parameters() != nullptr);
    REQUIRE(parsed.note_parameters()->size() == 64U);
    REQUIRE(parsed.note_parameters()->at(0)->sound_index() == 255);
    REQUIRE(parsed.note_parameters()->at(1)->sound_index() == 0);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    REQUIRE(writeStream.str() == originalBytes);
}

TEST_CASE("Kaitai MPC3000 PGM parses a real hardware empty PROGRAM.pgm", "[kaitai-pgm][real-mpc3000]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Pgm/PROGRAM.pgm");
    const std::string bytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        bytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_pgm_v3_t parsed(&parseIo);
    parsed._read();

    REQUIRE(static_cast<unsigned char>(bytes[0]) == 0x07);
    REQUIRE(static_cast<unsigned char>(bytes[1]) == 0x00);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 128U);
    REQUIRE(parsed.sound_sizes() != nullptr);
    REQUIRE(parsed.sound_sizes()->size() == 128U);
    REQUIRE(parsed.sound_assignments() != nullptr);
    REQUIRE(parsed.sound_assignments()->size() == 64U);
    REQUIRE(parsed.mixer_screens() != nullptr);
    REQUIRE(parsed.mixer_screens()->size() == 64U);
    REQUIRE(parsed.pad_note_number_assignments() != nullptr);
    REQUIRE(parsed.pad_note_number_assignments()->size() == 64U);

    REQUIRE(parsed.sound_names()->at(0)->name().empty());
    REQUIRE(parsed.sound_sizes()->at(0) == 0U);
    REQUIRE(parsed.program_name().substr(0, 10) == "PROGRAM_01");

    REQUIRE(parsed.note_variation_screen()->note_number_assignment() == 34U);
    REQUIRE(parsed.note_variation_screen()->tuning_low_range() == 136U);
    REQUIRE(parsed.note_variation_screen()->tuning_hi_range() == 120U);
    REQUIRE(parsed.note_variation_screen()->attack_low_range() == 12U);
    REQUIRE(parsed.note_variation_screen()->attack_hi_range() == 45U);
    REQUIRE(parsed.note_variation_screen()->decay_low_range() == 0U);
    REQUIRE(parsed.note_variation_screen()->decay_hi_range() == 20U);
    REQUIRE(parsed.note_variation_screen()->filter_low_range() == 206U);
    REQUIRE(parsed.note_variation_screen()->filter_hi_range() == 50U);

    REQUIRE(parsed.effects_screen()->effects_on() == false);
    REQUIRE(parsed.effects_screen()->delay_volume_tap1() == 50U);
    REQUIRE(parsed.effects_screen()->delay_volume_tap2() == 0U);
    REQUIRE(parsed.effects_screen()->delay_volume_tap3() == 0U);
    REQUIRE(parsed.effects_screen()->delay_pan_tap1() == 50U);
    REQUIRE(parsed.effects_screen()->delay_pan_tap2() == 50U);
    REQUIRE(parsed.effects_screen()->delay_pan_tap3() == 50U);
    REQUIRE(parsed.effects_screen()->delay_msecs_tap1() == 100U);
    REQUIRE(parsed.effects_screen()->delay_msecs_tap2() == 200U);
    REQUIRE(parsed.effects_screen()->delay_msecs_tap3() == 300U);
    REQUIRE(parsed.effects_screen()->delay_feedback_tap1() == 0U);
    REQUIRE(parsed.effects_screen()->delay_feedback_tap2() == 0U);
    REQUIRE(parsed.effects_screen()->delay_feedback_tap3() == 0U);

    REQUIRE(parsed.sound_assignments()->at(0)->sound_number() == 255U);
    REQUIRE(parsed.sound_assignments()->at(0)->sound_generator_mode() == mpc3000_pgm_v3_t::SOUND_GENERATOR_MODE_NORMAL);
    REQUIRE(parsed.sound_assignments()->at(0)->if_over1() == 44U);
    REQUIRE(parsed.sound_assignments()->at(0)->use_also_plays1() == 34U);
    REQUIRE(parsed.sound_assignments()->at(0)->if_over2() == 88U);
    REQUIRE(parsed.sound_assignments()->at(0)->use_also_plays2() == 34U);
    REQUIRE(parsed.sound_assignments()->at(0)->poly() == mpc3000_pgm_v3_t::POLY_MODE_POLY);
    REQUIRE(parsed.sound_assignments()->at(0)->cutoff_note_1() == 34U);
    REQUIRE(parsed.sound_assignments()->at(0)->cutoff_note_2() == 34U);
    REQUIRE(parsed.sound_assignments()->at(0)->tune() == 0);
    REQUIRE(parsed.sound_assignments()->at(0)->attack() == 0U);
    REQUIRE(parsed.sound_assignments()->at(0)->decay() == 6U);
    REQUIRE(parsed.sound_assignments()->at(0)->decay_mode() == mpc3000_pgm_v3_t::DECAY_MODE_END);
    REQUIRE(parsed.sound_assignments()->at(0)->filter_frequency() == 100U);
    REQUIRE(parsed.sound_assignments()->at(0)->filter_resonance() == 0U);
    REQUIRE(parsed.sound_assignments()->at(0)->filter_envel_attack() == 0U);
    REQUIRE(parsed.sound_assignments()->at(0)->filter_envel_decay() == 0U);
    REQUIRE(parsed.sound_assignments()->at(0)->filter_envel_amount() == 0U);
    REQUIRE(parsed.sound_assignments()->at(0)->veloc_mod_of_volume() == 100U);
    REQUIRE(parsed.sound_assignments()->at(0)->veloc_mod_of_attack() == 0U);
    REQUIRE(parsed.sound_assignments()->at(0)->veloc_mod_of_soft_start() == 0U);
    REQUIRE(parsed.sound_assignments()->at(0)->veloc_mod_of_filter_freq() == 0U);
    REQUIRE(parsed.sound_assignments()->at(0)->param() == mpc3000_pgm_v3_t::NOTE_VARIATION_TYPE_TUNE);

    REQUIRE(parsed.mixer_screens()->at(0)->stereo_mix_volume() == 100U);
    REQUIRE(parsed.mixer_screens()->at(0)->stereo_mix_pan() == 50U);
    REQUIRE(parsed.mixer_screens()->at(0)->echo_volume() == 0U);
    REQUIRE(parsed.mixer_screens()->at(0)->out_assign() == mpc3000_pgm_v3_t::INDIVIDUAL_OUT_INTERNAL_EFFECTS_GENERATOR);
    REQUIRE(parsed.mixer_screens()->at(0)->follow_stereo() == false);

    REQUIRE(parsed.pad_note_number_assignments()->at(0)->note_number() == 37U);
    REQUIRE(parsed.pad_note_number_assignments()->at(1)->note_number() == 36U);
    REQUIRE(parsed.pad_note_number_assignments()->at(2)->note_number() == 42U);
    REQUIRE(parsed.pad_note_number_assignments()->at(3)->note_number() == 82U);
    REQUIRE(parsed.pad_note_number_assignments()->at(63)->note_number() == 98U);

}

TEST_CASE("Kaitai MPC3000 PGM decodes a probed SIMULT generator-mode variant", "[kaitai-pgm][real-mpc3000]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Pgm/PROGRAM_01_SIMULT.pgm");
    const std::string bytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        bytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_pgm_v3_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.program_name().substr(0, 10) == "PROGRAM_01");

    size_t simultCount = 0;
    for (const auto &assignment : *parsed.sound_assignments()) {
        if (assignment->sound_generator_mode() == mpc3000_pgm_v3_t::SOUND_GENERATOR_MODE_SIMULTANEOUS) {
            simultCount++;
        }
    }

    REQUIRE(simultCount == 1U);
}

TEST_CASE("Kaitai MPC3000 PGM decodes a probed VEL SW generator-mode variant", "[kaitai-pgm][real-mpc3000]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Pgm/PROGRAM_02_VELSW.pgm");
    const std::string bytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        bytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_pgm_v3_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.program_name().substr(0, 10) == "PROGRAM_02");

    size_t velocitySwitchCount = 0;
    for (const auto &assignment : *parsed.sound_assignments()) {
        if (assignment->sound_generator_mode() == mpc3000_pgm_v3_t::SOUND_GENERATOR_MODE_VELOCITY_SWITCH) {
            velocitySwitchCount++;
        }
    }

    REQUIRE(velocitySwitchCount == 1U);
}

TEST_CASE("Kaitai MPC3000 PGM decodes a probed VEL SW threshold and target variant", "[kaitai-pgm][real-mpc3000]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Pgm/PROGRAM_03_VELSW_43.pgm");
    const std::string bytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        bytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_pgm_v3_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.program_name().substr(0, 10) == "PROGRAM_03");

    size_t velocitySwitchCount = 0;
    const mpc3000_pgm_v3_t::sound_assignment_t* switchedAssignment = nullptr;
    for (const auto &assignment : *parsed.sound_assignments()) {
        if (assignment->sound_generator_mode() == mpc3000_pgm_v3_t::SOUND_GENERATOR_MODE_VELOCITY_SWITCH) {
            velocitySwitchCount++;
            switchedAssignment = assignment.get();
        }
    }

    REQUIRE(velocitySwitchCount == 1U);
    REQUIRE(switchedAssignment != nullptr);
    REQUIRE(switchedAssignment->if_over1() == 43U);
    REQUIRE(switchedAssignment->use_also_plays1() == 35U);
    REQUIRE(switchedAssignment->if_over2() == 88U);
    REQUIRE(switchedAssignment->use_also_plays2() == 34U);
    REQUIRE(switchedAssignment->poly() == mpc3000_pgm_v3_t::POLY_MODE_POLY);
    REQUIRE(switchedAssignment->cutoff_note_1() == 34U);
    REQUIRE(switchedAssignment->cutoff_note_2() == 34U);
    REQUIRE(switchedAssignment->param() == mpc3000_pgm_v3_t::NOTE_VARIATION_TYPE_TUNE);
}

TEST_CASE("Kaitai MPC3000 PGM captures raw tune and decay-mode bytes from an Env/Veloc probe", "[kaitai-pgm][real-mpc3000]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Pgm/PROGRAM_04_ENV_PROBE.pgm");
    const std::string bytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        bytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc3000_pgm_v3_t parsed(&parseIo);
    parsed._read();

    const auto* assignment = parsed.sound_assignments()->at(2).get();
    REQUIRE(assignment != nullptr);
    REQUIRE(assignment->sound_generator_mode() == mpc3000_pgm_v3_t::SOUND_GENERATOR_MODE_VELOCITY_SWITCH);
    REQUIRE(assignment->if_over1() == 43U);
    REQUIRE(assignment->use_also_plays1() == 35U);
    REQUIRE(assignment->if_over2() == 87U);
    REQUIRE(assignment->use_also_plays2() == 35U);
    REQUIRE(assignment->poly() == mpc3000_pgm_v3_t::POLY_MODE_NOTE_OFF);
    REQUIRE(assignment->cutoff_note_1() == 64U);
    REQUIRE(assignment->cutoff_note_2() == 65U);
    // This fixture preserves the raw bytes proven live on MPC3000 v3.10:
    // visible Tune:-1 wrote 0xffff and visible Dcy md:START wrote byte 1.
    REQUIRE(assignment->tune() == -1);
    REQUIRE(assignment->decay_mode() == mpc3000_pgm_v3_t::DECAY_MODE_START);
}

TEST_CASE("Kaitai MPC3000 PGM loads through the production seam with imported semantics", "[kaitai-pgm][real-mpc3000]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc3000/Pgm/PROGRAM_04_ENV_PROBE.pgm");
    std::vector<char> bytes(file.begin(), file.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    std::vector<std::string> soundNames;
    const auto program = loadProgramViaPgmIoFromBytes(mpc, "PROGRAM_04.PGM", bytes, soundNames);

    REQUIRE(program != nullptr);
    REQUIRE(program->getName() == "PROGRAM_04");
    REQUIRE(soundNames.empty());

    REQUIRE(program->getPad(0)->getNote() == 37);
    REQUIRE(program->getPad(1)->getNote() == 36);
    REQUIRE(program->getPad(2)->getNote() == 42);
    REQUIRE(program->getPad(3)->getNote() == 82);

    const auto note37 = program->getNoteParameters(37);
    REQUIRE(note37->getSoundIndex() == -1);
    REQUIRE(note37->getSoundGenerationMode() == mpc::sampler::SoundGenerationMode::VelocitySwitch);
    REQUIRE(note37->getVelocityRangeLower() == 43);
    REQUIRE(note37->getVelocityRangeUpper() == 87);
    REQUIRE(note37->getOptionalNoteA() == 35);
    REQUIRE(note37->getOptionalNoteB() == 35);
    REQUIRE(note37->getMuteAssignA() == 64);
    REQUIRE(note37->getMuteAssignB() == 65);
    REQUIRE(note37->getTune() == -1);
    REQUIRE(note37->getAttack() == 0);
    REQUIRE(note37->getDecay() == 0);
    REQUIRE(note37->getDecayMode() == 1);
    REQUIRE(note37->getFilterFrequency() == 100);
    REQUIRE(note37->getFilterResonance() == 0);
    REQUIRE(note37->getFilterAttack() == 0);
    REQUIRE(note37->getFilterDecay() == 0);
    REQUIRE(note37->getFilterEnvelopeAmount() == 0);
    REQUIRE(note37->getVeloToLevel() == 100);
    REQUIRE(note37->getVelocityToAttack() == 0);
    REQUIRE(note37->getVelocityToStart() == 0);
    REQUIRE(note37->getVelocityToFilterFrequency() == 0);
    REQUIRE(note37->getSliderParameterNumber() == 0);
    REQUIRE(note37->getVelocityToPitch() == 0);
    REQUIRE(note37->getVoiceOverlapMode() == mpc::sampler::VoiceOverlapMode::NOTE_OFF);

    const auto stereoMixer = note37->getStereoMixer();
    const auto indivFxMixer = note37->getIndivFxMixer();
    REQUIRE(stereoMixer->getLevel() == 100);
    REQUIRE(stereoMixer->getPanning() == 50);
    REQUIRE(indivFxMixer->getVolumeIndividualOut() == 0);
    REQUIRE(indivFxMixer->getOutput() == 0);
    REQUIRE(indivFxMixer->getFxPath() == 1);
    REQUIRE(indivFxMixer->getFxSendLevel() == 0);
    REQUIRE(indivFxMixer->isFollowingStereo() == false);

    const auto slider = program->getSlider();
    REQUIRE(slider->getNote() == 34);
    REQUIRE(slider->getTuneLowRange() == -120);
    REQUIRE(slider->getTuneHighRange() == 120);
    REQUIRE(slider->getDecayLowRange() == 0);
    REQUIRE(slider->getDecayHighRange() == 20);
    REQUIRE(slider->getAttackLowRange() == 12);
    REQUIRE(slider->getAttackHighRange() == 45);
    REQUIRE(slider->getFilterLowRange() == -50);
    REQUIRE(slider->getFilterHighRange() == 50);
}

TEST_CASE("Kaitai MPC3000 PGM compacts the 128-entry sound table for import", "[kaitai-pgm][real-mpc3000]")
{
    const auto bytes = rewriteRealMpc3000ProgramWithMutations(
        "test/RealMpc3000/Pgm/PROGRAM.pgm",
        [](mpc3000_pgm_v3_t& parsed)
        {
            parsed.sound_names()->at(0)->set_name(mpc3000SoundNameField("KICK"));
            parsed.sound_names()->at(5)->set_name(mpc3000SoundNameField("SNARE"));

            parsed.sound_assignments()->at(0)->set_sound_number(5);
            parsed.sound_assignments()->at(1)->set_sound_number(0);
            parsed.sound_assignments()->at(2)->set_sound_number(255);
            parsed.sound_assignments()->at(3)->set_sound_number(255);
        }
    );

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    std::vector<std::string> soundNames;
    const auto program =
        loadProgramViaPgmIoFromBytes(mpc, "PROGRAM_05.PGM", bytes, soundNames);

    REQUIRE(program != nullptr);
    REQUIRE(soundNames.size() == 2U);
    REQUIRE(soundNames[0] == "SNARE");
    REQUIRE(soundNames[1] == "KICK");
}

TEST_CASE("Kaitai MPC2000 PGM saves and reloads PROGRAM1 without handwritten code", "[kaitai-pgm]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto program = loadProgram1(mpc);
    REQUIRE(program != nullptr);
    REQUIRE(program->getName() == "PROGRAM1");

    const auto kaitaiBytes = mpc::file::kaitai::PgmIo::saveProgram(*program, mpc.getSampler());

    std::stringstream parseStream(
        std::string(kaitaiBytes.begin(), kaitaiBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x07\x04", 2));
    REQUIRE(parsed.sound_count() == 2);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 2U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 6) == "sound1");
    REQUIRE(parsed.sound_names()->at(1).substr(0, 6) == "sound2");
    REQUIRE(parsed.name().substr(0, 8) == "PROGRAM1");
    REQUIRE(parsed.note_parameters() != nullptr);
    REQUIRE(parsed.note_parameters()->at(0)->sound_index() == 0);
    REQUIRE(parsed.note_parameters()->at(1)->sound_index() == 1);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    REQUIRE(writeStream.str() == std::string(kaitaiBytes.begin(), kaitaiBytes.end()));

    auto disk = mpc.getDisk();
    auto kaitaiFile = disk->newFile("KPGM.PGM");
    std::vector<char> kaitaiBytesVec(kaitaiBytes.begin(), kaitaiBytes.end());
    kaitaiFile->setFileData(kaitaiBytesVec);
    disk->initFiles();

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto reparsedProgram = sampler->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    (void)mpc::disk::ProgramLoader::loadProgram(mpc, kaitaiFile, reparsedProgram, 0);
    mpc.getEngineHost()->prepareProcessBlock(512);

    REQUIRE(reparsedProgram != nullptr);
    REQUIRE(reparsedProgram->getName() == "KPGM");
    REQUIRE(sampler->getSoundCount() == 2);
    REQUIRE(sampler->getSoundName(0) == "sound1");
    REQUIRE(sampler->getSoundName(1) == "sound2");
    REQUIRE(reparsedProgram->getNoteParameters(35)->getSoundIndex() == 0);
    REQUIRE(reparsedProgram->getNoteParameters(36)->getSoundIndex() == 1);
}

TEST_CASE("Kaitai MPC2000 PGM loads PROGRAM1 through the production seam with explicit semantics", "[kaitai-pgm]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    const auto program = loadProgram1(mpc);
    requireProgram1LoadedState(mpc, program);
}

TEST_CASE("Kaitai MPC2000 PGM load uses standalone byte as MIDI program change", "[kaitai-pgm]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();
    parsed.set_program_change(1);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    std::vector<char> programBytes(rewrittenBytes.begin(), rewrittenBytes.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto program = loadProgram1FromBytesWithoutRender(mpc, programBytes);

    REQUIRE(program->getMidiProgramChange() == 2);
    REQUIRE(program->getSlider()->getControlChange() == 0);
}

TEST_CASE("Kaitai MPC2000 PGM save writes MIDI program change and ignores slider control change", "[kaitai-pgm]")
{
    mpc::Mpc mpc;
        mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto program = loadProgram1(mpc);

    program->setMidiProgramChange(2);
    mpc.getPerformanceManager().lock()->drainQueue();
    program->getSlider()->setControlChange(0);
    const auto bytesWithZeroControl =
        mpc::file::kaitai::PgmIo::saveProgram(*program, mpc.getSampler());

    std::stringstream parseStreamZero(
        std::string(bytesWithZeroControl.begin(), bytesWithZeroControl.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIoZero(&parseStreamZero);
    mpc2000xl_pgm_t parsedZero(&parseIoZero);
    parsedZero._read();
    REQUIRE(parsedZero.program_change() == 1);

    program->getSlider()->setControlChange(17);
    const auto bytesWithNonZeroControl =
        mpc::file::kaitai::PgmIo::saveProgram(*program, mpc.getSampler());

    REQUIRE(bytesWithNonZeroControl == bytesWithZeroControl);
}

TEST_CASE("Kaitai MPC2000 PGM load preserves mixer effects send level", "[kaitai-pgm]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();

    parsed.pad_mixers()->at(1)->set_effects_send_level(50);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    std::vector<char> programBytes(rewrittenBytes.begin(), rewrittenBytes.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto program = loadProgram1FromBytesWithoutRender(mpc, programBytes);

    REQUIRE(program != nullptr);
    REQUIRE(program->getNoteParameters(36)->getIndivFxMixer()->getFxSendLevel() == 50);
}

TEST_CASE("Kaitai MPC2000 PGM direct load handles upper-bound MIDI program change", "[kaitai-pgm]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();
    parsed.set_program_change(127);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    std::vector<char> programBytes(rewrittenBytes.begin(), rewrittenBytes.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    std::vector<std::string> soundNames;
    const auto program = loadProgram1ViaPgmIoFromBytes(mpc, programBytes, soundNames);
    REQUIRE(program->getMidiProgramChange() == 128);
    REQUIRE(soundNames.size() == 2U);
}

TEST_CASE("Kaitai MPC2000 PGM direct parse reads upper-bound MIDI program change byte", "[kaitai-pgm]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();
    parsed.set_program_change(127);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    std::stringstream reparsedStream(
        rewrittenBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream reparsedIo(&reparsedStream);
    mpc2000xl_pgm_t reparsed(&reparsedIo);
    reparsed._read();
    REQUIRE(reparsed.program_change() == 127);
}

TEST_CASE("Kaitai MPC2000 PGM direct load preserves broad mutated program semantics", "[kaitai-pgm]")
{
    const auto programBytes = rewriteProgram1WithMutations(applyBroadProgramMutation);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    std::vector<std::string> soundNames;
    const auto program = loadProgram1ViaPgmIoFromBytes(mpc, programBytes, soundNames);
    requireBroadMutatedProgramState(program, soundNames);
}

TEST_CASE("Kaitai MPC2000 PGM direct load preserves mutated note tune", "[kaitai-pgm]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();
    parsed.note_parameters()->at(0)->set_tune(-120);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    std::vector<char> programBytes(rewrittenBytes.begin(), rewrittenBytes.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    std::vector<std::string> soundNames;
    const auto program = loadProgram1ViaPgmIoFromBytes(mpc, programBytes, soundNames);
    REQUIRE(program->getNoteParameters(35)->getTune() == -120);
}

TEST_CASE("ProgramLoader loads upper-bound MIDI program change", "[kaitai-pgm]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();
    parsed.set_program_change(127);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    std::vector<char> programBytes(rewrittenBytes.begin(), rewrittenBytes.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const auto program = loadProgram1FromBytes(mpc, programBytes);

    REQUIRE(program != nullptr);
    REQUIRE(program->getMidiProgramChange() == 128);
}

TEST_CASE("ProgramLoader loads mutated note tune", "[kaitai-pgm]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ProgramLoading/program1/PROGRAM1.PGM");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_pgm_t parsed(&parseIo);
    parsed._read();
    parsed.note_parameters()->at(0)->set_tune(-120);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    std::vector<char> programBytes(rewrittenBytes.begin(), rewrittenBytes.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const auto program = loadProgram1FromBytes(mpc, programBytes);

    REQUIRE(program != nullptr);
    REQUIRE(program->getNoteParameters(35)->getTune() == -120);
}

TEST_CASE("Kaitai MPC2000 PGM save and production load preserve broad mutated program semantics", "[kaitai-pgm]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    const auto mutatedBytes = rewriteProgram1WithMutations(applyBroadProgramMutation);
    const auto mutatedProgram = loadProgram1FromBytesWithoutRender(mpc, mutatedBytes);
    const std::vector<std::string> expectedSoundNames{
        std::string("sound1") + std::string(10, ' '),
        std::string("sound2") + std::string(10, ' ')
    };
    requireBroadMutatedProgramState(mutatedProgram, expectedSoundNames);

    const auto savedBytes = mpc::file::kaitai::PgmIo::saveProgram(*mutatedProgram, mpc.getSampler());
    const auto reparsedProgram = loadProgram1FromBytesWithoutRender(mpc, savedBytes);
    requireBroadMutatedProgramState(reparsedProgram, expectedSoundNames);
}

TEST_CASE("ProgramLoader loads broad mutated program semantics", "[kaitai-pgm]")
{
    const auto programBytes = rewriteProgram1WithMutations(applyBroadProgramMutation);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    const auto program = loadProgram1FromBytesWithoutRender(mpc, programBytes);
    const std::vector<std::string> expectedSoundNames{
        std::string("sound1") + std::string(10, ' '),
        std::string("sound2") + std::string(10, ' ')
    };

    requireBroadMutatedProgramState(program, expectedSoundNames);
}
