#include <catch2/catch_test_macros.hpp>

#include "Util.hpp"
#include "Utf8.hpp"
#include "mpc_fs.hpp"

#include <chrono>
#include <string>
#include <vector>

TEST_CASE("MPC display text keeps its UTF-8 bytes", "[cxx20]")
{
    CHECK(mpc::Util::replaceDotWithSmallSpaceDot("120.0") ==
          std::string("120") + "\xC3\x8B" + "0");
    CHECK(mpc::Util::distributeTimeSig("3/4") ==
          std::string("\xC3\x8E") + "3" + "\xC3\x8E" + "/" +
              "\xC3\x8E" + "4");
}

TEST_CASE("MPC filesystem reads a non-ASCII path", "[cxx20]")
{
    const auto temp = mpc_fs::temp_directory_path();
    REQUIRE(temp);

    const auto unique = std::chrono::steady_clock::now()
                            .time_since_epoch()
                            .count();
    const auto path = *temp /
                      ("vmpc-cxx20-" + std::to_string(unique) +
                       mpc::utf8(u8"-\u00E9.txt"));
    const std::string payload = "filesystem compatibility";

    REQUIRE(set_file_data(path, payload));
    const auto data = get_file_data(path);
    REQUIRE(data);
    CHECK(std::string(data->begin(), data->end()) == payload);
    CHECK(mpc_fs::remove(path));
}
