#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/ApsLoader.hpp"
#include "disk/MpcFile.hpp"
#include "engine/IndivFxMixer.hpp"
#include "engine/EngineHost.hpp"
#include "engine/StereoMixer.hpp"
#include "file/kaitai/ApsIo.hpp"
#include "file/kaitai/generated/mpc2000xl_aps.h"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "lcdgui/screens/PgmAssignScreen.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

#include <cmrc/cmrc.hpp>
#include <kaitai/kaitaistream.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <sstream>
#include <string>
#include <string_view>

CMRC_DECLARE(mpctest);

namespace {

std::vector<char> rewriteAllPgmsApsWithMutations(
    const std::function<void(mpc2000xl_aps_t&)>& mutate)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ApsLoading/ALL_PGMS.APS");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
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

std::string describeFirstByteDiff(
    const std::string& lhs,
    const std::string& rhs)
{
    const auto mismatch = std::mismatch(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    if (mismatch.first == lhs.end() && mismatch.second == rhs.end())
    {
        return "no diff";
    }

    const auto offset = static_cast<size_t>(mismatch.first - lhs.begin());
    std::ostringstream oss;
    oss << "first diff at offset " << offset
        << ": lhs=0x"
        << std::hex << std::setw(2) << std::setfill('0')
        << (static_cast<unsigned>(static_cast<unsigned char>(*mismatch.first)))
        << " rhs=0x"
        << std::hex << std::setw(2) << std::setfill('0')
        << (static_cast<unsigned>(static_cast<unsigned char>(*mismatch.second)));
    return oss.str();
}

void applyBroadApsMutation(mpc2000xl_aps_t& parsed)
{
    auto* program0 = parsed.aps_programs()->at(0)->body();
    auto* note35 = program0->note_parameters()->at(0).get();
    note35->set_sound_index(0xFFFF);
    note35->set_sound_generation_mode(mpc2000xl_pgm_t::SOUND_GENERATION_MODE_VEL_SW);
    note35->set_velocity_range_lower(1);
    note35->set_also_play_use_note_1(41);
    note35->set_velocity_range_upper(127);
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

    auto* mixer35 = program0->pad_mixers()->at(0).get();
    mixer35->set_fx_output(mpc2000xl_pgm_t::FX_OUTPUT_R2);
    mixer35->set_volume(12);
    mixer35->set_pan(13);
    mixer35->set_volume_individual(14);
    mixer35->set_output(2);
    mixer35->set_effects_send_level(15);

    program0->slider()->set_note(52);
    program0->slider()->set_tune_low(-119);
    program0->slider()->set_tune_high(118);
    program0->slider()->set_decay_low(11);
    program0->slider()->set_decay_high(12);
    program0->slider()->set_attack_low(13);
    program0->slider()->set_attack_high(14);
    program0->slider()->set_filter_low(-15);
    program0->slider()->set_filter_high(16);
    program0->set_program_change(126);
    program0->pad_to_note_mapping()->at(0) = 50;
    parsed.master_pad_to_note_mapping()->at(0) = 50;

    auto* drum0 = parsed.drum1();
    drum0->set_receive_program_change(mpc2000xl_aps_t::NO_YES_FALSE);
    drum0->set_receive_midi_volume(mpc2000xl_aps_t::NO_YES_FALSE);
    drum0->set_program(1);
    drum0->set_receive_program_change_duplicate(mpc2000xl_aps_t::NO_YES_FALSE);
    drum0->set_receive_midi_volume_duplicate(mpc2000xl_aps_t::NO_YES_FALSE);

    auto* globals = parsed.global_parameters();
    globals->set_pad_to_internal_sound(mpc2000xl_aps_t::NO_YES_TRUE);
    globals->set_pad_assign(mpc2000xl_aps_t::PAD_ASSIGN_MASTERS);
    globals->set_indiv_fx_source(mpc2000xl_aps_t::MIX_SOURCE_DRUM);
    globals->set_stereo_mix_source(mpc2000xl_aps_t::MIX_SOURCE_DRUM);
    globals->set_record_mix_changes(mpc2000xl_aps_t::NO_YES_TRUE);
    globals->set_copy_pgm_mix_to_drum(mpc2000xl_aps_t::NO_YES_FALSE);
    globals->set_fx_drum(3);
    globals->set_master_level(2);
}

void prepareApsResources(mpc::Mpc& mpc)
{
    auto disk = mpc.getDisk();
    auto fs = cmrc::mpctest::get_filesystem();

    for (auto&& entry : fs.iterate_directory("test/ApsLoading")) {
        auto file = fs.open("test/ApsLoading/" + entry.filename());
        std::vector<char> data(file.begin(), file.end());
        auto newFile = disk->newFile(entry.filename());
        newFile->setFileData(data);
    }

    disk->initFiles();
}

std::shared_ptr<mpc::disk::MpcFile> installApsResourceFile(
    mpc::Mpc& mpc,
    const std::string& resourcePath,
    const std::string& fileName)
{
    auto disk = mpc.getDisk();
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open(resourcePath);
    std::vector<char> data(file.begin(), file.end());
    auto newFile = disk->newFile(fileName);
    newFile->setFileData(data);
    disk->initFiles();
    return newFile;
}

void loadAllPgmsAps(mpc::Mpc& mpc)
{
    prepareApsResources(mpc);
    auto apsFile = mpc.getDisk()->getFile("ALL_PGMS.APS");
    constexpr bool headless = true;
    mpc::disk::ApsLoader::load(mpc, apsFile, headless);
    mpc.getEngineHost()->prepareProcessBlock(512);
}

std::shared_ptr<mpc::disk::MpcFile> loadAllPgmsApsFileFromBytes(
    mpc::Mpc& mpc,
    const std::vector<char>& apsBytes)
{
    prepareApsResources(mpc);
    auto apsFile = mpc.getDisk()->getFile("ALL_PGMS.APS");
    auto mutableBytes = apsBytes;
    apsFile->setFileData(mutableBytes);
    mpc.getDisk()->initFiles();
    return apsFile;
}

void loadAllPgmsApsWithoutRender(mpc::Mpc& mpc, const std::vector<char>& apsBytes)
{
    auto apsFile = loadAllPgmsApsFileFromBytes(mpc, apsBytes);
    constexpr bool headless = true;
    mpc::disk::ApsLoader::load(mpc, apsFile, headless);
    mpc.getPerformanceManager().lock()->drainQueue();
}

}

TEST_CASE("Kaitai MPC2000 APS parses and rewrites ALL_PGMS", "[kaitai-aps]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ApsLoading/ALL_PGMS.APS");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x0a\x05", 2));
    REQUIRE(parsed.sound_count() == 3);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 3U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 6) == "sound1");
    REQUIRE(parsed.sound_names()->at(1).substr(0, 6) == "sound2");
    REQUIRE(parsed.sound_names()->at(2).substr(0, 6) == "sound3");
    REQUIRE(parsed.aps_programs() != nullptr);
    REQUIRE(parsed.aps_programs()->size() >= 2U);
    REQUIRE(parsed.aps_programs()->at(0)->index() == 0);
    REQUIRE(parsed.aps_programs()->at(0)->body() != nullptr);
    REQUIRE(parsed.aps_programs()->at(0)->body()->name().substr(0, 8) == "PROGRAM1");
    REQUIRE(parsed.aps_programs()->at(0)->body()->note_parameters()->at(0)->sound_index() == 0);
    REQUIRE(parsed.aps_programs()->at(0)->body()->note_parameters()->at(1)->sound_index() == 1);
    REQUIRE(parsed.aps_programs()->at(1)->index() == 1);
    REQUIRE(parsed.aps_programs()->at(1)->body() != nullptr);
    REQUIRE(parsed.aps_programs()->at(1)->body()->name().substr(0, 8) == "PROGRAM2");
    REQUIRE(parsed.aps_programs()->at(1)->body()->note_parameters()->at(0)->sound_index() == 1);
    REQUIRE(parsed.aps_programs()->at(1)->body()->note_parameters()->at(1)->sound_index() == 2);
    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    REQUIRE(rewrittenBytes.size() == originalBytes.size());
    INFO(describeFirstByteDiff(rewrittenBytes, originalBytes));
    REQUIRE(std::equal(rewrittenBytes.begin(), rewrittenBytes.end(), originalBytes.begin()));
}

TEST_CASE("Kaitai MPC2000 APS parses and rewrites real 2KXL ALL_PGMS", "[kaitai-aps][real-2kxl]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/RealMpc2000xl/Aps/ALL_PGMS.APS");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x0a\x05", 2));
    REQUIRE(parsed.sound_count() == 2);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->size() == 2U);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 1) == "M");
    REQUIRE(parsed.sound_names()->at(1).substr(0, 2) == "ML");
    REQUIRE(parsed.name().substr(0, 8) == "ALL_PGMS");
    REQUIRE(parsed.aps_programs() != nullptr);
    REQUIRE(parsed.aps_programs()->size() >= 1U);
    REQUIRE(parsed.aps_programs()->at(0)->index() == 0);
    REQUIRE(parsed.aps_programs()->at(0)->body() != nullptr);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    REQUIRE(rewrittenBytes.size() == originalBytes.size());
    INFO(describeFirstByteDiff(rewrittenBytes, originalBytes));
    REQUIRE(std::equal(rewrittenBytes.begin(), rewrittenBytes.end(), originalBytes.begin()));
}

TEST_CASE("Kaitai MPC2000 APS saves and reloads ALL_PGMS through production seam", "[kaitai-aps]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    loadAllPgmsAps(mpc);

    REQUIRE(mpc.getSampler()->getProgramCount() == 2);
    REQUIRE(mpc.getSampler()->getSoundCount() == 3);

    const auto directKaitaiBytes = mpc::file::kaitai::ApsIo::save(mpc, "ALL_PGMS");

    std::stringstream parseStream(
        std::string(directKaitaiBytes.begin(), directKaitaiBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.magic() == std::string("\x0a\x05", 2));
    REQUIRE(parsed.sound_count() == 3);
    REQUIRE(parsed.sound_names() != nullptr);
    REQUIRE(parsed.sound_names()->at(0).substr(0, 6) == "sound1");
    REQUIRE(parsed.sound_names()->at(1).substr(0, 6) == "sound2");
    REQUIRE(parsed.sound_names()->at(2).substr(0, 6) == "sound3");
    REQUIRE(parsed.aps_programs() != nullptr);
    REQUIRE(parsed.aps_programs()->at(0)->body() != nullptr);
    REQUIRE(parsed.aps_programs()->at(0)->body()->name().substr(0, 8) == "PROGRAM1");
    REQUIRE(parsed.aps_programs()->at(1)->body() != nullptr);
    REQUIRE(parsed.aps_programs()->at(1)->body()->name().substr(0, 8) == "PROGRAM2");

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto kaitaiBytes = writeStream.str();
    REQUIRE(kaitaiBytes.size() == directKaitaiBytes.size());
    INFO(describeFirstByteDiff(
        kaitaiBytes,
        std::string(directKaitaiBytes.begin(), directKaitaiBytes.end())));
    REQUIRE(std::equal(kaitaiBytes.begin(), kaitaiBytes.end(), directKaitaiBytes.begin()));

    auto sampler = mpc.getSampler();
    sampler->deleteAllPrograms(false);
    sampler->deleteAllSamples();

    auto disk = mpc.getDisk();
    auto kaitaiFile = disk->newFile("KAPS.APS");
    std::vector<char> kaitaiBytesVec(kaitaiBytes.begin(), kaitaiBytes.end());
    kaitaiFile->setFileData(kaitaiBytesVec);
    disk->initFiles();

    constexpr bool headless = true;
    mpc::disk::ApsLoader::load(mpc, kaitaiFile, headless);
    mpc.getEngineHost()->prepareProcessBlock(512);

    auto p1 = sampler->getProgram(0);
    auto p2 = sampler->getProgram(1);
    REQUIRE(sampler->getProgramCount() == 2);
    REQUIRE(p1->getName() == "PROGRAM1");
    REQUIRE(p2->getName() == "PROGRAM2");
    REQUIRE(sampler->getSoundCount() == 3);
    REQUIRE(sampler->getSoundName(0) == "sound1");
    REQUIRE(sampler->getSoundName(1) == "sound2");
    REQUIRE(sampler->getSoundName(2) == "sound3");
    REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == 0);
    REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == 1);
    REQUIRE(p2->getNoteParameters(35)->getSoundIndex() == 1);
    REQUIRE(p2->getNoteParameters(36)->getSoundIndex() == 2);
}

TEST_CASE("Kaitai MPC2000 APS saves MIDI program change separately from slider control change", "[kaitai-aps]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    loadAllPgmsAps(mpc);

    auto p1 = mpc.getSampler()->getProgram(0);
    REQUIRE(p1 != nullptr);

    p1->setMidiProgramChange(2);
    mpc.getPerformanceManager().lock()->drainQueue();
    REQUIRE(p1->getMidiProgramChange() == 2);
    p1->getSlider()->setControlChange(0);

    const auto bytesWithZeroControl =
        mpc::file::kaitai::ApsIo::save(mpc, "ALL_PGMS");

    std::stringstream parseStreamZero(
        std::string(bytesWithZeroControl.begin(), bytesWithZeroControl.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIoZero(&parseStreamZero);
    mpc2000xl_aps_t parsedZero(&parseIoZero);
    parsedZero._read();
    REQUIRE(parsedZero.aps_programs()->at(0)->body()->program_change() == 1);

    p1->getSlider()->setControlChange(17);
    const auto bytesWithNonZeroControl =
        mpc::file::kaitai::ApsIo::save(mpc, "ALL_PGMS");

    REQUIRE(bytesWithNonZeroControl == bytesWithZeroControl);
}

TEST_CASE("Kaitai MPC2000 APS loads real 2KXL ALL_PGMS through production seam", "[kaitai-aps][real-2kxl]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    auto apsFile = installApsResourceFile(
        mpc,
        "test/RealMpc2000xl/Aps/ALL_PGMS.APS",
        "ALL_PGMS.APS"
    );
    REQUIRE(apsFile);

    constexpr bool headless = true;
    mpc::file::kaitai::ApsIo::load(mpc, apsFile, headless);
    mpc.getEngineHost()->prepareProcessBlock(512);

    auto sampler = mpc.getSampler();
    REQUIRE(sampler->getProgramCount() == 1);
    REQUIRE(sampler->getSoundCount() == 0);

    auto p1 = sampler->getProgram(0);
    REQUIRE(p1 != nullptr);
    REQUIRE(p1->getName() == "NewPgm-A");
    REQUIRE(p1->getMidiProgramChange() == 1);
    REQUIRE(p1->getSlider()->getControlChange() == 0);
    REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == -1);
    REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == -1);
}

TEST_CASE("Kaitai APS rewrite preserves upper-bound MIDI program change", "[kaitai-aps]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ApsLoading/ALL_PGMS.APS");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();
    parsed.aps_programs()->at(0)->body()->set_program_change(127);

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
    mpc2000xl_aps_t reparsed(&reparsedIo);
    reparsed._read();
    REQUIRE(reparsed.aps_programs()->at(0)->body()->program_change() == 127);
}

TEST_CASE("ApsLoader loads upper-bound MIDI program change", "[kaitai-aps]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ApsLoading/ALL_PGMS.APS");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();
    parsed.aps_programs()->at(0)->body()->set_program_change(127);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    std::vector<char> apsBytes(rewrittenBytes.begin(), rewrittenBytes.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    loadAllPgmsApsWithoutRender(mpc, apsBytes);

    auto p1 = mpc.getSampler()->getProgram(0);
    REQUIRE(p1 != nullptr);
    REQUIRE(p1->getMidiProgramChange() == 128);
}

TEST_CASE("ApsLoader loads mutated note tune", "[kaitai-aps]")
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open("test/ApsLoading/ALL_PGMS.APS");
    const std::string originalBytes(
        std::string_view(file.begin(), file.end() - file.begin())
    );

    std::stringstream parseStream(
        originalBytes,
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();
    parsed.aps_programs()->at(0)->body()->note_parameters()->at(0)->set_tune(-120);

    std::stringstream writeStream(std::ios::in | std::ios::out | std::ios::binary);
    kaitai::kstream writeIo(&writeStream);
    parsed._set_io(&writeIo);
    parsed._check();
    parsed._write();

    const auto rewrittenBytes = writeStream.str();
    std::vector<char> apsBytes(rewrittenBytes.begin(), rewrittenBytes.end());

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    loadAllPgmsApsWithoutRender(mpc, apsBytes);

    auto p1 = mpc.getSampler()->getProgram(0);
    REQUIRE(p1 != nullptr);
    REQUIRE(p1->getNoteParameters(35)->getTune() == -120);
}

TEST_CASE("ApsLoader maps all four APS drum buses to distinct MPC drum buses", "[kaitai-aps]")
{
    const auto apsBytes = rewriteAllPgmsApsWithMutations([](mpc2000xl_aps_t& parsed)
    {
        parsed.drum1()->set_program(1);
        parsed.drum1()->set_receive_program_change(mpc2000xl_aps_t::NO_YES_FALSE);
        parsed.drum1()->set_receive_midi_volume(mpc2000xl_aps_t::NO_YES_TRUE);

        parsed.drum2()->set_program(2);
        parsed.drum2()->set_receive_program_change(mpc2000xl_aps_t::NO_YES_TRUE);
        parsed.drum2()->set_receive_midi_volume(mpc2000xl_aps_t::NO_YES_FALSE);

        parsed.drum3()->set_program(3);
        parsed.drum3()->set_receive_program_change(mpc2000xl_aps_t::NO_YES_FALSE);
        parsed.drum3()->set_receive_midi_volume(mpc2000xl_aps_t::NO_YES_FALSE);

        parsed.drum4()->set_program(4);
        parsed.drum4()->set_receive_program_change(mpc2000xl_aps_t::NO_YES_TRUE);
        parsed.drum4()->set_receive_midi_volume(mpc2000xl_aps_t::NO_YES_TRUE);
    });

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    loadAllPgmsApsWithoutRender(mpc, apsBytes);

    auto drum0 = mpc.getSequencer()->getDrumBus(mpc::DrumBusIndex(0));
    auto drum1 = mpc.getSequencer()->getDrumBus(mpc::DrumBusIndex(1));
    auto drum2 = mpc.getSequencer()->getDrumBus(mpc::DrumBusIndex(2));
    auto drum3 = mpc.getSequencer()->getDrumBus(mpc::DrumBusIndex(3));

    REQUIRE(drum0->getProgramIndex() == 1);
    REQUIRE(!drum0->receivesPgmChange());
    REQUIRE(drum0->receivesMidiVolume());

    REQUIRE(drum1->getProgramIndex() == 2);
    REQUIRE(drum1->receivesPgmChange());
    REQUIRE(!drum1->receivesMidiVolume());

    REQUIRE(drum2->getProgramIndex() == 3);
    REQUIRE(!drum2->receivesPgmChange());
    REQUIRE(!drum2->receivesMidiVolume());

    REQUIRE(drum3->getProgramIndex() == 4);
    REQUIRE(drum3->receivesPgmChange());
    REQUIRE(drum3->receivesMidiVolume());
}

TEST_CASE("Kaitai APS parses broad mutated APS semantics", "[kaitai-aps]")
{
    const auto apsBytes = rewriteAllPgmsApsWithMutations(applyBroadApsMutation);
    INFO("raw pad_assign byte=0x"
        << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<unsigned>(static_cast<unsigned char>(apsBytes.at(75))));
    INFO("raw mix-source byte=0x"
        << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<unsigned>(static_cast<unsigned char>(apsBytes.at(76))));
    INFO("raw record/copy byte=0x"
        << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<unsigned>(static_cast<unsigned char>(apsBytes.at(77))));
    INFO("raw slider note byte=0x"
        << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<unsigned>(static_cast<unsigned char>(apsBytes.at(1757))));

    std::stringstream parseStream(
        std::string(apsBytes.begin(), apsBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.aps_programs()->size() >= 1U);

    auto* p1 = parsed.aps_programs()->at(0)->body();
    REQUIRE(p1->name().substr(0, 8) == "PROGRAM1");
    REQUIRE(p1->pad_to_note_mapping()->at(0) == 50);
    REQUIRE(p1->slider()->note() == 52);
    REQUIRE(p1->slider()->tune_low() == -119);
    REQUIRE(p1->slider()->tune_high() == 118);
    REQUIRE(p1->slider()->decay_low() == 11);
    REQUIRE(p1->slider()->decay_high() == 12);
    REQUIRE(p1->slider()->attack_low() == 13);
    REQUIRE(p1->slider()->attack_high() == 14);
    REQUIRE(p1->slider()->filter_low() == -15);
    REQUIRE(p1->slider()->filter_high() == 16);
    REQUIRE(p1->program_change() == 126);

    auto* note35 = p1->note_parameters()->at(0).get();
    REQUIRE(note35->sound_index() == 0xFFFF);
    REQUIRE(note35->sound_generation_mode() == mpc2000xl_pgm_t::SOUND_GENERATION_MODE_VEL_SW);
    REQUIRE(note35->velocity_range_lower() == 1);
    REQUIRE(note35->also_play_use_note_1() == 41);
    REQUIRE(note35->velocity_range_upper() == 127);
    REQUIRE(note35->also_play_use_note_2() == 42);
    REQUIRE(note35->voice_overlap_mode() == mpc2000xl_pgm_t::VOICE_OVERLAP_MODE_NOTE_OFF);
    REQUIRE(note35->mute_assign_1() == 43);
    REQUIRE(note35->mute_assign_2() == 44);
    REQUIRE(note35->tune() == -120);
    REQUIRE(note35->attack() == 99);
    REQUIRE(note35->decay() == 98);
    REQUIRE(note35->decay_mode() == mpc2000xl_pgm_t::DECAY_MODE_START);
    REQUIRE(note35->cutoff() == 17);
    REQUIRE(note35->resonance() == 18);
    REQUIRE(note35->velocity_envelope_to_filter_attack() == 19);
    REQUIRE(note35->velocity_envelope_to_filter_decay() == 20);
    REQUIRE(note35->velocity_envelope_to_filter_amount() == 21);
    REQUIRE(note35->velocity_to_level() == 22);
    REQUIRE(note35->velocity_to_attack() == 23);
    REQUIRE(note35->velocity_to_start() == 24);
    REQUIRE(note35->velocity_to_cutoff() == 25);
    REQUIRE(note35->slider_parameter() == mpc2000xl_pgm_t::SLIDER_PARAMETER_FILTER);
    REQUIRE(note35->velocity_to_pitch() == 26);

    auto* drum0 = parsed.drum1();
    REQUIRE(drum0->program() == 1);
    REQUIRE(drum0->receive_program_change() == mpc2000xl_aps_t::NO_YES_FALSE);
    REQUIRE(drum0->receive_midi_volume() == mpc2000xl_aps_t::NO_YES_FALSE);

    auto* globals = parsed.global_parameters();
    REQUIRE(globals->pad_to_internal_sound() == mpc2000xl_aps_t::NO_YES_TRUE);
    REQUIRE(globals->pad_assign() == mpc2000xl_aps_t::PAD_ASSIGN_MASTERS);
    REQUIRE(globals->indiv_fx_source() == mpc2000xl_aps_t::MIX_SOURCE_DRUM);
    REQUIRE(globals->stereo_mix_source() == mpc2000xl_aps_t::MIX_SOURCE_DRUM);
    REQUIRE(globals->record_mix_changes() == mpc2000xl_aps_t::NO_YES_TRUE);
    REQUIRE(globals->copy_pgm_mix_to_drum() == mpc2000xl_aps_t::NO_YES_FALSE);
    REQUIRE(globals->fx_drum() == 3);
    REQUIRE(globals->master_level() == 2);
}

TEST_CASE("Kaitai APS saves maximum sound count in 16-bit header", "[kaitai-aps]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    for (uint16_t i = 0; i < 256; ++i)
    {
        auto sound = mpc.getSampler()->addSound();
        REQUIRE(sound != nullptr);
        sound->setName("S" + std::to_string(i));
    }

    const auto apsBytes = mpc::file::kaitai::ApsIo::save(mpc, "MAXSOUNDS");

    std::stringstream parseStream(
        std::string(apsBytes.begin(), apsBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.sound_count() == 256);
    REQUIRE(parsed.sound_names()->size() == 256U);
}

TEST_CASE("Kaitai APS saves unassigned pad mapping", "[kaitai-aps]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);

    const auto manager = mpc.getPerformanceManager().lock();
    mpc.getSampler()
        ->getProgram(mpc::MinProgramIndex)
        ->getPad(mpc::MinProgramPadIndex)
        ->setNote(mpc::NoDrumNoteAssigned);
    manager->drainQueue();

    const auto apsBytes = mpc::file::kaitai::ApsIo::save(mpc, "UNASSIGN");

    std::stringstream parseStream(
        std::string(apsBytes.begin(), apsBytes.end()),
        std::ios::in | std::ios::out | std::ios::binary
    );
    kaitai::kstream parseIo(&parseStream);
    mpc2000xl_aps_t parsed(&parseIo);
    parsed._read();

    REQUIRE(parsed.aps_programs()->at(0)->body()->pad_to_note_mapping()->at(0) == 34);
}

TEST_CASE("ApsLoader loads broad mutated APS semantics", "[kaitai-aps]")
{
    const auto apsBytes = rewriteAllPgmsApsWithMutations(applyBroadApsMutation);

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpcWithoutMidiServices(mpc);
    loadAllPgmsApsWithoutRender(mpc, apsBytes);

    auto sampler = mpc.getSampler();
    auto p1 = sampler->getProgram(0);
    REQUIRE(p1 != nullptr);
    REQUIRE(p1->getPad(0)->getNote() == 50);
    REQUIRE(p1->getMidiProgramChange() == 127);

    auto note35 = p1->getNoteParameters(35);
    REQUIRE(note35->getSoundIndex() == -1);
    REQUIRE(note35->getSoundGenerationMode() == 2);
    REQUIRE(note35->getVelocityRangeLower() == 1);
    REQUIRE(note35->getOptionalNoteA() == 41);
    REQUIRE(note35->getVelocityRangeUpper() == 127);
    REQUIRE(note35->getOptionalNoteB() == 42);
    REQUIRE(note35->getVoiceOverlapMode() == mpc::sampler::VoiceOverlapMode::NOTE_OFF);
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

    auto stereoMixer = note35->getStereoMixer();
    auto indivFxMixer = note35->getIndivFxMixer();
    REQUIRE(stereoMixer->getLevel() == 12);
    REQUIRE(stereoMixer->getPanning() == 13);
    REQUIRE(indivFxMixer->getVolumeIndividualOut() == 14);
    REQUIRE(indivFxMixer->getOutput() == 2);
    REQUIRE(indivFxMixer->getFxPath() == 4);
    REQUIRE(indivFxMixer->getFxSendLevel() == 15);

    auto slider = p1->getSlider();
    REQUIRE(slider->getNote() == 52);
    REQUIRE(slider->getTuneLowRange() == -119);
    REQUIRE(slider->getTuneHighRange() == 118);
    REQUIRE(slider->getDecayLowRange() == 11);
    REQUIRE(slider->getDecayHighRange() == 12);
    REQUIRE(slider->getAttackLowRange() == 13);
    REQUIRE(slider->getAttackHighRange() == 14);
    REQUIRE(slider->getFilterLowRange() == -15);
    REQUIRE(slider->getFilterHighRange() == 16);

    auto drum0 = mpc.getSequencer()->getDrumBus(mpc::DrumBusIndex(0));
    REQUIRE(drum0->getProgramIndex() == 1);
    REQUIRE(!drum0->receivesPgmChange());
    REQUIRE(!drum0->receivesMidiVolume());

    auto mixerSetup = mpc.screens->get<mpc::lcdgui::ScreenId::MixerSetupScreen>();
    REQUIRE(mixerSetup->isRecordMixChangesEnabled());
    REQUIRE(!mixerSetup->isCopyPgmMixToDrumEnabled());
    REQUIRE(mixerSetup->getFxDrum() == 3);
    REQUIRE(mixerSetup->isIndivFxSourceDrum());
    REQUIRE(mixerSetup->isStereoMixSourceDrum());
    REQUIRE(mixerSetup->getMasterLevel() == 2);

    auto drumScreen = mpc.screens->get<mpc::lcdgui::ScreenId::DrumScreen>();
    REQUIRE(drumScreen->isPadToIntSound());

    auto pgmAssignScreen = mpc.screens->get<mpc::lcdgui::ScreenId::PgmAssignScreen>();
    REQUIRE(pgmAssignScreen->isPadAssignMaster());
}
