#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include "hardware2/HardwareComponent.h"

using namespace mpc::hardware2;

TEST_CASE("Button responds to press and release", "[hardware2]") {
    Button b;
    REQUIRE_NOTHROW(b.press());
    REQUIRE_NOTHROW(b.release());
}

TEST_CASE("Pad responds to press, aftertouch, and release", "[hardware2]") {
    Pad p;
    REQUIRE_NOTHROW(p.pressWithVelocity(VelocitySensitivePressable::MAX_VELO));   // velocity press
    REQUIRE_NOTHROW(p.press());       // binary press
    REQUIRE_NOTHROW(p.aftertouch(1));
    REQUIRE_NOTHROW(p.release());
}

TEST_CASE("DataWheel turns correctly", "[hardware2]") {
    DataWheel wheel;
    REQUIRE_NOTHROW(wheel.turn(1));
    REQUIRE_NOTHROW(wheel.turn(-2));
}

TEST_CASE("Slider clamps values between 0 and 127", "[hardware2]") {
    Slider s;
    s.setValue(50);
    REQUIRE(s.getValue() == 50);
    s.setValue(-1);
    REQUIRE(s.getValue() == 0);
    s.setValue(128);
    REQUIRE(s.getValue() == 127);
}

TEST_CASE("Pot clamps values between 0 and 100", "[hardware2]") {
    Pot p;
    p.setValue(50);
    REQUIRE(p.getValue() == 50);
    p.setValue(-1);
    REQUIRE(p.getValue() == 0);
    p.setValue(101);
    REQUIRE(p.getValue() == 100);
}

TEST_CASE("Pad is in pressed and non-pressed state correctly", "[hardware2]")
{
    Pad p;
    REQUIRE(!p.isPressed());
    p.press();
    REQUIRE(p.isPressed());
    p.release();
    REQUIRE(!p.isPressed());

    for (int i = 1; i <= 127; i++)
    {
        p.pressWithVelocity(i);
        REQUIRE(p.isPressed());
        p.release();
        REQUIRE(!p.isPressed());
    }

    REQUIRE_THROWS_AS(p.pressWithVelocity(0), std::invalid_argument);
}

TEST_CASE("Pad registers velocity and aftertouch pressure correctly", "[hardware2]")
{
    Pad p;
    REQUIRE(!p.getLastVelocity().has_value());
    REQUIRE(!p.getLastPressure().has_value());
    p.press();
    REQUIRE(*p.getLastVelocity() == VelocitySensitivePressable::MAX_VELO);
    REQUIRE(*p.getLastPressure() == Aftertouchable::MAX_PRESSURE);

    p.release();

    REQUIRE(*p.getLastVelocity() == VelocitySensitivePressable::MAX_VELO);
    REQUIRE(*p.getLastPressure() == Aftertouchable::MAX_PRESSURE);

    for (int velo = 1; velo <= 127; velo++)
    {
        p.pressWithVelocity(velo);
        REQUIRE(*p.getLastVelocity() == velo);
        REQUIRE(*p.getLastPressure() == velo);
        for (int pressure = 1; pressure <= 127; pressure++)
        {
            p.aftertouch(pressure);
        }
        p.release();
    }

    REQUIRE_THROWS_AS(p.pressWithVelocity(0), std::invalid_argument);
}

