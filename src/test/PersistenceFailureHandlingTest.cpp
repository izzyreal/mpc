#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "TestMpc.hpp"
#include "TestPaths.hpp"
#include "input/keyboard/KeyboardBindingsReader.hpp"
#include "input/midi/MidiControlPresetUtil.hpp"
#include "nvram/VolumesPersistence.hpp"

using namespace mpc;
using namespace mpc::input::keyboard;

namespace
{
void prepareTestPaths(Mpc &mpc)
{
    mpc.paths = std::make_shared<TestPaths>();
    const auto docs = mpc.paths->getDocuments()->appDocumentsPath();
    (void) mpc_fs::create_directories(docs);
    (void) mpc_fs::create_directories(mpc.paths->configPath());
    (void) mpc_fs::create_directories(
        mpc.paths->getDocuments()->midiControlPresetsPath());
}
}

TEST_CASE("Corrupt keyboard bindings file returns a safe error",
          "[persistence]")
{
    {
        Mpc seed;
        TestMpc::resetTestDataRoot(seed);
    }

    Mpc mpc;
    prepareTestPaths(mpc);

    const auto writeRes = set_file_data(
        mpc.paths->keyboardBindingsPath(),
        std::string("{\"version\": 3, \"bindings\": ["));
    REQUIRE(writeRes);

    const auto bindingsRes =
        KeyboardBindingsReader::fromJsonFile(mpc.paths->keyboardBindingsPath());
    CHECK_FALSE(bindingsRes.has_value());
}

TEST_CASE("Corrupt active MIDI preset file returns a safe error",
          "[persistence]")
{
    {
        Mpc seed;
        TestMpc::resetTestDataRoot(seed);
    }

    Mpc mpc;
    prepareTestPaths(mpc);

    const auto presetPath =
        mpc.paths->getDocuments()->activeMidiControlPresetPath();
    const auto writeRes =
        set_file_data(presetPath, std::string("{\"name\": \"broken\""));
    REQUIRE(writeRes);

    const auto presetRes =
        input::midi::MidiControlPresetUtil::loadPresetFromFile(presetPath);
    CHECK_FALSE(presetRes.has_value());
}

TEST_CASE("Malformed volumes persistence does not crash and yields defaults",
          "[persistence]")
{
    {
        Mpc seed;
        TestMpc::resetTestDataRoot(seed);
    }

    Mpc mpc;
    prepareTestPaths(mpc);

    const auto path = mpc.paths->configPath() / "volumes.json";
    const auto writeRes = set_file_data(path, std::string("{not-json"));
    REQUIRE(writeRes);

    REQUIRE_NOTHROW(
        (void) mpc::nvram::VolumesPersistence::getPersistedActiveUUID(mpc));
    REQUIRE_NOTHROW(
        (void) mpc::nvram::VolumesPersistence::getPersistedConfigs(mpc));

    CHECK(mpc::nvram::VolumesPersistence::getPersistedActiveUUID(mpc).empty());
    CHECK(mpc::nvram::VolumesPersistence::getPersistedConfigs(mpc).empty());
}
