#include "catch2/catch_test_macros.hpp"
#include "input/legacy/LegacyKeyboardBindingsConvertor.hpp"

#include <nlohmann/json.hpp>

#include <cmrc/cmrc.hpp>

#include <string>
#include <iostream>

CMRC_DECLARE(mpctest);

using nlohmann::json;

inline std::string load_resource(const std::string &path)
{
    auto fs = cmrc::mpctest::get_filesystem();
    auto file = fs.open(path);
    return std::string(file.begin(), file.end());
}

TEST_CASE("Legacy preset V1 conversion works",
          "[legacy-keyboard-binding-v1-conversion]")
{
    auto data = load_resource("test/LegacyKeyboardBindingsV1/keys.txt");

    auto bindings = mpc::input::legacy::LegacyKeyboardBindingsConvertor::
        parseLegacyKeyboardBindings(data);

    REQUIRE(data.size() == 78);
}
