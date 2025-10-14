#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <stdexcept>
#include "hardware2/HardwareComponent.h"

using namespace mpc::hardware2;

using Catch::Approx;

TEST_CASE("Button responds to press and release", "[hardware2]")
{
    Button b("");
    REQUIRE_NOTHROW(b.press());
    REQUIRE_NOTHROW(b.release());
}

TEST_CASE("Button pressed state toggles correctly", "[hardware2]")
{
    Button b("TEST");

    REQUIRE(!b.isPressed());
    b.press();
    REQUIRE(b.isPressed());
    b.release();
    REQUIRE(!b.isPressed());
}

TEST_CASE("Led enables and disables correctly", "[hardware2]")
{
    Led led("LED1");

    REQUIRE(!led.isEnabled());
    led.setEnabled(true);
    REQUIRE(led.isEnabled());
    led.setEnabled(false);
    REQUIRE(!led.isEnabled());
}

TEST_CASE("Aftertouchable throws on invalid pressures", "[hardware2]")
{
    struct TestAftertouchable : mpc::hardware2::Aftertouchable {
        void onAftertouch(int) override {} // no-op for test
    };

    TestAftertouchable t;

    REQUIRE_THROWS_AS(t.aftertouch(0), std::invalid_argument);
    REQUIRE_THROWS_AS(t.aftertouch(128), std::invalid_argument);

    REQUIRE_NOTHROW(t.aftertouch(1));
    REQUIRE_NOTHROW(t.aftertouch(127));
}

TEST_CASE("Pad responds to press, aftertouch, and release", "[hardware2]")
{
    Pad p(0);
    REQUIRE_NOTHROW(p.pressWithVelocity(VelocitySensitivePressable::MAX_VELO));
    REQUIRE_NOTHROW(p.press());
    REQUIRE_NOTHROW(p.aftertouch(1));
    REQUIRE_NOTHROW(p.release());
}

TEST_CASE("DataWheel turns correctly", "[hardware2]")
{
    DataWheel wheel;
    REQUIRE_NOTHROW(wheel.turn(1));
    REQUIRE_NOTHROW(wheel.turn(-2));
}

TEST_CASE("Continuous type contracts", "[hardware2]")
{
    struct TestContinuous : mpc::hardware2::Continuous<float, 0, 127> {};
    TestContinuous c;

    STATIC_REQUIRE(std::is_same_v<decltype(c.getValue()), float>);
    STATIC_REQUIRE(std::is_same_v<decltype(c.getValueAs<int>()), int>);
    STATIC_REQUIRE(std::is_same_v<decltype(c.getRange()), std::pair<int, int>>);
    STATIC_REQUIRE(std::is_same_v<decltype(c.getRangeAs<double>()), std::pair<double, double>>);
}

TEST_CASE("Continuous<float> stores and clamps correctly", "[hardware2]")
{
    struct TestContinuous : mpc::hardware2::Continuous<float, 0, 127> {};
    TestContinuous c;

    c.setValue(63.5f);
    REQUIRE(c.getValue() == Approx(63.5f));

    c.setValue(-10.0f);
    REQUIRE(c.getValue() == 0.0f);

    c.setValue(999.9f);
    REQUIRE(c.getValue() == 127.0f);
}

TEST_CASE("Continuous<double> stores and clamps correctly", "[hardware2]")
{
    struct TestContinuous : mpc::hardware2::Continuous<double, 0, 127> {};
    TestContinuous c;

    c.setValue(63.5);
    REQUIRE(c.getValue() == Approx(63.5));

    c.setValue(-100.0);
    REQUIRE(c.getValue() == 0.0);

    c.setValue(1000.0);
    REQUIRE(c.getValue() == 127.0);
}

TEST_CASE("Continuous<int> clamps correctly", "[hardware2]")
{
    struct TestContinuous : mpc::hardware2::Continuous<int, 0, 127> {};
    TestContinuous c;

    c.setValue(63);
    REQUIRE(c.getValue() == 63);

    c.setValue(-5);
    REQUIRE(c.getValue() == 0);

    c.setValue(999);
    REQUIRE(c.getValue() == 127);

    c.setValue(0);
    REQUIRE(c.getValue() == 0);

    c.setValue(127);
    REQUIRE(c.getValue() == 127);
}

TEST_CASE("Continuous getValueAs<int> rounds correctly", "[hardware2]")
{
    struct TestContinuous : mpc::hardware2::Continuous<float, 0, 127> {};
    TestContinuous c;

    c.setValue(63.4f);
    REQUIRE(c.getValueAs<int>() == 63);

    c.setValue(63.6f);
    REQUIRE(c.getValueAs<int>() == 64);
}

TEST_CASE("Continuous getRange and getRangeAs work", "[hardware2]")
{
    struct TestContinuous : mpc::hardware2::Continuous<float, 0, 127> {};
    TestContinuous c;

    auto [imin, imax] = c.getRange();
    REQUIRE(imin == 0);
    REQUIRE(imax == 127);

    auto [fmin, fmax] = c.getRangeAs<float>();
    REQUIRE(fmin == Approx(0.0f));
    REQUIRE(fmax == Approx(127.0f));
}

TEST_CASE("Slider clamps values between 0 and 127", "[hardware2]")
{
    Slider s;
    s.setValue(50);
    REQUIRE(s.getValue() == 50);
    s.setValue(-1);
    REQUIRE(s.getValue() == 0);
    s.setValue(128);
    REQUIRE(s.getValue() == 127);
}

TEST_CASE("Slider direction logic works", "[hardware2]")
{
    Slider s;

    s.setDirection(Slider::Direction::UpIncreases);
    s.moveToNormalizedY(0.5f);
    s.moveToNormalizedY(0.0f);
    REQUIRE(s.getValueAs<int>() == 127);

    s.setDirection(Slider::Direction::DownIncreases);
    s.moveToNormalizedY(0.5f);
    s.moveToNormalizedY(0.0f);
    REQUIRE(s.getValueAs<int>() == 0);
}

TEST_CASE("Slider is a Continuous<float, 0, 127> specialization", "[hardware2]")
{
    using SliderBase = Continuous<float, 0, 127>;
    STATIC_REQUIRE(std::is_base_of_v<SliderBase, Slider>);
}

TEST_CASE("Pot clamps values between 0 and 127", "[hardware2]")
{
    Pot p;
    p.setValue(50);
    REQUIRE(p.getValue() == 50);
    p.setValue(-1);
    REQUIRE(p.getValue() == 0);
    p.setValue(128);
    REQUIRE(p.getValue() == 127);
}

TEST_CASE("Pad is in pressed and non-pressed state correctly", "[hardware2]")
{
    Pad p(0);
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

TEST_CASE("Pad registers velocity and aftertouch pressure correctly", "[hardware2]")
{
    Pad p(0);
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


TEST_CASE("Pad inherits from Pressable and VelocitySensitivePressable", "[hardware2]")
{
    STATIC_REQUIRE(std::is_base_of_v<Pressable, Pad>);
    STATIC_REQUIRE(std::is_base_of_v<VelocitySensitivePressable, Pad>);
}

TEST_CASE("Button inherits from Pressable", "[hardware2]")
{
    STATIC_REQUIRE(std::is_base_of_v<Pressable, Button>);
}

TEST_CASE("Pressable press and doublePress return correct bool", "[hardware2]")
{
    struct TestPressable : Pressable {};

    TestPressable p;

    REQUIRE(p.press() == true);
    REQUIRE(p.isPressed() == true);
    REQUIRE(p.press() == false); // Already pressed
    REQUIRE(p.doublePress() == false); // Already pressed

    p.release();
    REQUIRE(p.isPressed() == false);
    REQUIRE(p.doublePress() == true);
    REQUIRE(p.isPressed() == true);
    REQUIRE(p.doublePress() == false); // Already pressed
}

TEST_CASE("VelocitySensitivePressable pressWithVelocity returns correct bool", "[hardware2]")
{
    struct TestVelocitySensitivePressable : VelocitySensitivePressable {};

    TestVelocitySensitivePressable vsp;

    REQUIRE(vsp.pressWithVelocity(64) == true);
    REQUIRE(vsp.isPressed() == true);
    REQUIRE(vsp.pressWithVelocity(64) == false); // Already pressed

    vsp.release();
    REQUIRE(vsp.isPressed() == false);
    REQUIRE(vsp.pressWithVelocity(127) == true);
    REQUIRE(vsp.isPressed() == true);
}

