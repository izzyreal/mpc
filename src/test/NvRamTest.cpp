#include <catch2/catch_test_macros.hpp>
#include "mpc_fs.hpp"
#include "Paths.hpp"
#include "TestMpc.hpp"
#include "file/all/AllParser.hpp"

TEST_CASE("defaults", "[defaults]")
{
    fs::path configPath;

    for (int i = 0; i < 10; i++)
    {
        {
            mpc::Mpc m;
            mpc::TestMpc::initializeTestMpc(m);
            configPath = m.paths->configPath();
        }

        auto nvramPath = configPath / "nvram.vmp";
        INFO("nvramPath: " << nvramPath);

        REQUIRE(fs::file_size(nvramPath) == mpc::file::all::AllParser::DEFAULTS_LENGTH);
    }
}

