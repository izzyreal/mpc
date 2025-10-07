#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <vector>
#include <string>
#include "audiomidi/VmpcMidiControlMode.hpp"
#include "engine/midi/ShortMessage.hpp"
#include "hardware/DataWheel.hpp"
#include "hardware/HwPad.hpp"
#include "hardware/HwSlider.hpp"
#include "hardware/Pot.hpp"
#include "lcdgui/screens/IVmpcMidiScreen.h"
#include "nvram/MidiControlPersistence.hpp"

using namespace mpc::audiomidi;
using namespace mpc::engine::midi;
using namespace mpc::lcdgui::screens;
using namespace mpc::hardware;

// ----------------------------------------------------
// Mock helpers
// ----------------------------------------------------
struct MockPad {
    bool pressed = false;
    int lastPressure = -1;
    bool pressedFlag = false;
    bool releasedFlag = false;

    bool isPressed() { return pressed; }
    void setPressure(uint8_t value) { lastPressure = value; }
    void push(uint8_t = 0) { pressedFlag = true; }
    void release() { releasedFlag = true; }
};

struct MockDataWheel {
    int totalTurn = 0;
    void turn(int increment, bool = false) { totalTurn += increment; }
};

struct MockSlider {
    int value = -1;
    void setValue(int v) { value = v; }
};

struct MockPot {
    std::string label;
    int value = -1;
    explicit MockPot(std::string labelToUse) : label(std::move(labelToUse)) {}
    void setValue(int v) { value = v; }
};

struct MockHardware : public IHardware {
    std::vector<std::shared_ptr<MockPad>> pads;
    std::shared_ptr<MockDataWheel> dataWheel;
    std::shared_ptr<MockSlider> slider;
    std::shared_ptr<MockPot> recPot;
    std::shared_ptr<MockPot> volPot;

    MockHardware() {
        for (int i = 0; i < 16; i++)
            pads.push_back(std::make_shared<MockPad>());

        dataWheel = std::make_shared<MockDataWheel>();
        slider = std::make_shared<MockSlider>();
        recPot = std::make_shared<MockPot>("rec");
        volPot = std::make_shared<MockPot>("vol");
    }

    std::vector<std::shared_ptr<HwPad>>& getPads() override {
        static std::vector<std::shared_ptr<HwPad>> dummy;
        dummy.clear();
        return dummy;
    }

    std::shared_ptr<DataWheel> getDataWheel() override { return {}; }
    std::shared_ptr<Slider> getSlider() override { return {}; }
    std::shared_ptr<Pot> getRecPot() override { return {}; }
    std::shared_ptr<Pot> getVolPot() override { return {}; }
    std::shared_ptr<HwComponent> getComponentByLabel(const std::string&) override { return {}; }
};

// ----------------------------------------------------
// Mock IVmpcMidiScreen
// ----------------------------------------------------
struct MockScreen : public IVmpcMidiScreen {
    bool learning = false;
    bool candidateSet = false;
    std::shared_ptr<mpc::nvram::MidiControlPreset> preset;

    MockScreen() {
        preset = std::make_shared<mpc::nvram::MidiControlPreset>();
    }

    bool isLearning() override { return learning; }

    void setLearnCandidate(const bool isNote, const int8_t channelIndex, const int8_t number, const int8_t value) override
    {
        candidateSet = true;
    }

    std::shared_ptr<mpc::nvram::MidiControlPreset> getActivePreset() override
    {
        return preset;
    }
};

// ----------------------------------------------------
// Tests
// ----------------------------------------------------
TEST_CASE("Channel pressure updates only pressed pads", "[vmpc-midi-control-mode]")
{
    auto hw = std::make_shared<MockHardware>();
    hw->pads[0]->pressed = true;
    hw->pads[1]->pressed = false;
    auto screen = std::make_shared<MockScreen>();
    ShortMessage msg;
    msg.setMessage(ShortMessage::CHANNEL_PRESSURE, 0, 100, 0);

    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &msg);

    REQUIRE(hw->pads[0]->lastPressure == 100);
    REQUIRE(hw->pads[1]->lastPressure == -1);
}

TEST_CASE("Learning mode sets candidate only for NoteOn and CC", "[vmpc-midi-control-mode]")
{
    auto hw = std::make_shared<MockHardware>();
    auto screen = std::make_shared<MockScreen>();
    screen->learning = true;

    ShortMessage noteOn, cc, noteOff;
    noteOn.setMessage(ShortMessage::NOTE_ON, 0, 60, 127);
    cc.setMessage(ShortMessage::CONTROL_CHANGE, 0, 10, 64);
    noteOff.setMessage(ShortMessage::NOTE_OFF, 0, 60, 0);

    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &noteOn);
    REQUIRE(screen->candidateSet);

    screen->candidateSet = false;
    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &cc);
    REQUIRE(screen->candidateSet);

    screen->candidateSet = false;
    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &noteOff);
    REQUIRE_FALSE(screen->candidateSet);
}

TEST_CASE("Datawheel-up and datawheel-down cause correct turn directions", "[vmpc-midi-control-mode]")
{
    auto hw = std::make_shared<MockHardware>();
    auto screen = std::make_shared<MockScreen>();

    auto &rows = screen->preset->rows;
    mpc::nvram::MidiControlCommand cmdUp;
    cmdUp.setMpcHardwareLabel("datawheel-up");
    cmdUp.setMidiMessageType(mpc::nvram::MidiControlCommand::CC);
    rows.push_back(cmdUp);

    mpc::nvram::MidiControlCommand cmdDown;
    cmdDown.setMpcHardwareLabel("datawheel-down");
    cmdDown.setMidiMessageType(mpc::nvram::MidiControlCommand::CC);
    rows.push_back(cmdDown);

    ShortMessage msg;
    msg.setMessage(ShortMessage::CONTROL_CHANGE, 0, 10, 64);

    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &msg);
    REQUIRE(hw->dataWheel->totalTurn == 0);

    rows[0].setNumber(10);
    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &msg);
    REQUIRE(hw->dataWheel->totalTurn == 1);

    rows.clear();
    rows.push_back(cmdDown);
    rows[0].setNumber(10);
    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &msg);
    REQUIRE(hw->dataWheel->totalTurn == 0);
}

TEST_CASE("Slider, rec-gain, and main-volume get correct values", "[vmpc-midi-control-mode]")
{
    auto hw = std::make_shared<MockHardware>();
    auto screen = std::make_shared<MockScreen>();
    auto &rows = screen->preset->rows;

    auto sliderCmd = mpc::nvram::MidiControlCommand();
    sliderCmd.setMpcHardwareLabel("slider");
    sliderCmd.setMidiMessageType(mpc::nvram::MidiControlCommand::CC);
    sliderCmd.setNumber(1);
    rows.push_back(sliderCmd);

    auto recCmd = mpc::nvram::MidiControlCommand();
    recCmd.setMpcHardwareLabel("rec-gain");
    recCmd.setMidiMessageType(mpc::nvram::MidiControlCommand::CC);
    recCmd.setNumber(2);
    rows.push_back(recCmd);

    auto volCmd = mpc::nvram::MidiControlCommand();
    volCmd.setMpcHardwareLabel("main-volume");
    volCmd.setMidiMessageType(mpc::nvram::MidiControlCommand::CC);
    volCmd.setNumber(3);
    rows.push_back(volCmd);

    ShortMessage msg;
    msg.setMessage(ShortMessage::CONTROL_CHANGE, 0, 1, 64);
    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &msg);
    REQUIRE(hw->slider->value == 64);

    msg.setMessage(ShortMessage::CONTROL_CHANGE, 0, 2, 127);
    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &msg);
    REQUIRE(hw->recPot->value == 100);

    msg.setMessage(ShortMessage::CONTROL_CHANGE, 0, 3, 127);
    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &msg);
    REQUIRE(hw->volPot->value == 100);
}

TEST_CASE("Component push/release behavior for generic CC and NoteOn", "[vmpc-midi-control-mode]")
{
    auto hw = std::make_shared<MockHardware>();
    auto screen = std::make_shared<MockScreen>();
    auto &rows = screen->preset->rows;

    auto padCmd = mpc::nvram::MidiControlCommand();
    padCmd.setMpcHardwareLabel("pad1");
    padCmd.setMidiMessageType(mpc::nvram::MidiControlCommand::NOTE);
    padCmd.setNumber(60);
    rows.push_back(padCmd);

    ShortMessage msgOn;
    msgOn.setMessage(ShortMessage::NOTE_ON, 0, 60, 100);
    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &msgOn);
    REQUIRE(hw->pads[0]->pressedFlag);

    ShortMessage msgOff;
    msgOff.setMessage(ShortMessage::NOTE_ON, 0, 60, 0);
    VmpcMidiControlMode().processMidiInputEvent(hw, screen, &msgOff);
    REQUIRE(hw->pads[0]->releasedFlag);
}
