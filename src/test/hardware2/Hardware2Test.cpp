#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include "hardware2/HardwareComponent.h"
#include "inputlogic/ClientInputMapper.h"

using namespace mpc::hardware2;
using namespace mpc::inputlogic;

TEST_CASE("Button responds to press and release", "[hardware2]") {
    ClientInputMapper mapper;
    Button b(mapper, "");
    REQUIRE_NOTHROW(b.press());
    REQUIRE_NOTHROW(b.release());
}

TEST_CASE("Pad responds to press, aftertouch, and release", "[hardware2]") {
    ClientInputMapper mapper;
    Pad p(0, mapper);
    REQUIRE_NOTHROW(p.pressWithVelocity(VelocitySensitivePressable::MAX_VELO));
    REQUIRE_NOTHROW(p.press());
    REQUIRE_NOTHROW(p.aftertouch(1));
    REQUIRE_NOTHROW(p.release());
}

TEST_CASE("DataWheel turns correctly", "[hardware2]") {
    ClientInputMapper mapper;
    DataWheel wheel(mapper);
    REQUIRE_NOTHROW(wheel.turn(1));
    REQUIRE_NOTHROW(wheel.turn(-2));
}

TEST_CASE("Slider clamps values between 0 and 127", "[hardware2]") {
    ClientInputMapper mapper;
    Slider s(mapper);
    s.setValue(50);
    REQUIRE(s.getValue() == 50);
    s.setValue(-1);
    REQUIRE(s.getValue() == 0);
    s.setValue(128);
    REQUIRE(s.getValue() == 127);
}

TEST_CASE("Pot clamps values between 0 and 100", "[hardware2]") {
    ClientInputMapper mapper;
    Pot p(mapper);
    p.setValue(50);
    REQUIRE(p.getValue() == 50);
    p.setValue(-1);
    REQUIRE(p.getValue() == 0);
    p.setValue(101);
    REQUIRE(p.getValue() == 100);
}

TEST_CASE("Pad is in pressed and non-pressed state correctly", "[hardware2]") {
    ClientInputMapper mapper;
    Pad p(0, mapper);
    REQUIRE(!p.isPressed());
    p.press();
    REQUIRE(p.isPressed());
    p.release();
    REQUIRE(!p.isPressed());

    for (int i = 1; i <= 127; i++) {
        p.pressWithVelocity(i);
        REQUIRE(p.isPressed());
        p.release();
        REQUIRE(!p.isPressed());
    }

    REQUIRE_THROWS_AS(p.pressWithVelocity(0), std::invalid_argument);
}

TEST_CASE("Pad registers velocity and aftertouch pressure correctly", "[hardware2]") {
    ClientInputMapper mapper;
    Pad p(0, mapper);
    REQUIRE(!p.getVelocity().has_value());
    REQUIRE(!p.getPressure().has_value());
    p.press();
    REQUIRE(*p.getVelocity() == VelocitySensitivePressable::MAX_VELO);
    REQUIRE(!p.getPressure().has_value());

    p.release();

    REQUIRE(*p.getVelocity() == VelocitySensitivePressable::MAX_VELO);
    REQUIRE(!p.getPressure().has_value());

    for (int velo = 1; velo <= 127; velo++) {
        p.pressWithVelocity(velo);
        REQUIRE(*p.getVelocity() == velo);
        REQUIRE(!p.getPressure().has_value());
        for (int pressure = 1; pressure <= 127; pressure++) {
            p.aftertouch(pressure);
        }
        p.release();
    }

    REQUIRE_THROWS_AS(p.pressWithVelocity(0), std::invalid_argument);
}

