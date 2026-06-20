#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "IntTypes.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/PgmFileToProgramConverter.hpp"
#include "disk/ProgramLoader.hpp"
#include "engine/IndivFxMixer.hpp"
#include "engine/EngineHost.hpp"
#include "engine/StereoMixer.hpp"
#include "file/kaitai/PgmIo.hpp"
#include "file/kaitai/generated/mpc2000xl_pgm.h"
#include "file/pgmreader/ProgramFileReader.hpp"
#include "file/pgmreader/PRSlider.hpp"
#include "file/pgmwriter/PgmWriter.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/Pad.hpp"
#include "sampler/PgmSlider.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
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

void applyBroadProgramMutation(mpc2000xl_pgm_t& parsed)
{
    auto* note35 = parsed.note_parameters()->at(0).get();
    note35->set_sound_index(255);
    note35->set_sound_generation_mode(mpc2000xl_pgm_t::SOUND_GENERATION_MODE_VEL_SW);
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
    REQUIRE(note35->getSoundGenerationMode() == 2);
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

    REQUIRE(note35->getSoundGenerationMode() == 0);
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

TEST_CASE("Kaitai MPC2000 PGM matches handwritten PGM bytes", "[kaitai-pgm]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto program = loadProgram1(mpc);
    REQUIRE(program != nullptr);
    REQUIRE(program->getName() == "PROGRAM1");

    mpc::file::pgmwriter::PgmWriter handwrittenWriter(program.get(), mpc.getSampler());
    const auto handwrittenBytes = handwrittenWriter.get();

    std::stringstream parseStream(
        std::string(handwrittenBytes.begin(), handwrittenBytes.end()),
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

    const auto kaitaiBytes = writeStream.str();
    REQUIRE(kaitaiBytes == std::string(handwrittenBytes.begin(), handwrittenBytes.end()));

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

TEST_CASE("PgmFileToProgramConverter loads upper-bound MIDI program change", "[kaitai-pgm]")
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
    auto pgmFile = loadProgram1FileFromBytes(mpc, programBytes);
    auto program = mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    std::vector<std::string> soundNames;

    auto result = mpc::disk::PgmFileToProgramConverter::loadFromFileAndConvert(
        mpc, pgmFile, program, soundNames
    );
    REQUIRE(result);

    mpc.getPerformanceManager().lock()->drainQueue();
    REQUIRE(program->getMidiProgramChange() == 128);
}

TEST_CASE("ProgramFileReader reads upper-bound MIDI program change byte", "[kaitai-pgm]")
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
    auto pgmFile = loadProgram1FileFromBytes(mpc, programBytes);

    mpc::file::pgmreader::ProgramFileReader reader(pgmFile);
    REQUIRE(reader.getSlider()->getProgramChange() == 127);
}

TEST_CASE("PgmFileToProgramConverter loads mutated note tune", "[kaitai-pgm]")
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
    auto pgmFile = loadProgram1FileFromBytes(mpc, programBytes);
    auto program = mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    std::vector<std::string> soundNames;

    auto result = mpc::disk::PgmFileToProgramConverter::loadFromFileAndConvert(
        mpc, pgmFile, program, soundNames
    );
    REQUIRE(result);

    mpc.getPerformanceManager().lock()->drainQueue();
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

TEST_CASE("PgmFileToProgramConverter loads broad mutated program semantics", "[kaitai-pgm]")
{
    const auto programBytes = rewriteProgram1WithMutations(applyBroadProgramMutation);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutIoServices(mpc);
    auto pgmFile = loadProgram1FileFromBytes(mpc, programBytes);
    auto program = mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
    std::vector<std::string> soundNames;

    auto result = mpc::disk::PgmFileToProgramConverter::loadFromFileAndConvert(
        mpc, pgmFile, program, soundNames
    );
    REQUIRE(result);

    mpc.getPerformanceManager().lock()->drainQueue();
    requireBroadMutatedProgramState(program, soundNames);
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
