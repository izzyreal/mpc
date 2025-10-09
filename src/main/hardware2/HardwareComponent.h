#pragma once

#include <algorithm>
#include <string>
#include <cassert>
#include <stdexcept>
#include <optional>

#include <inputlogic/InputMapper.h>
#include <inputlogic/HardwareTranslator.h>
#include <inputlogic/InputAction.h>

namespace mpc::hardware2 {

class Component {
protected:
    mpc::inputlogic::InputMapper& mapper;
    explicit Component(mpc::inputlogic::InputMapper& mapperToUse) : mapper(mapperToUse) {}
public:
    virtual ~Component() = default;
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
};

class PressState {
    bool pressed = false;
protected:
    PressState() = default;
    void setPressed(bool pressedToUse) { pressed = pressedToUse; }
public:
    ~PressState() = default;
    bool isPressed() const { return pressed; }
};

class Pressable : public PressState {
protected:
    Pressable() = default;
    virtual void onPress() = 0;
    virtual void onRelease() = 0;
public:
    virtual ~Pressable() = default;

    void press() {
        setPressed(true);
        onPress();
    }

    void release() {
        setPressed(false);
        onRelease();
    }
};

class VelocitySensitivePressable : public Pressable {
    std::optional<int> lastVelocity;
protected:
    VelocitySensitivePressable() = default;
    virtual void onPressWithVelocity(int velocity) = 0;
public:
    static constexpr int MIN_VELO = 1;
    static constexpr int MAX_VELO = 127;

    virtual ~VelocitySensitivePressable() = default;

    void pressWithVelocity(int velocity) {
        if (velocity < MIN_VELO || velocity > MAX_VELO)
            throw std::invalid_argument("Velocity must be between " + std::to_string(MIN_VELO) + " and " + std::to_string(MAX_VELO));

        setPressed(true);
        lastVelocity = velocity;
        onPressWithVelocity(velocity);
    }

    std::optional<int> getLastVelocity() const { return lastVelocity; }
};

class Aftertouchable {
    std::optional<int> lastPressure;
protected:
    Aftertouchable() = default;
    virtual void onAftertouch(int pressure) = 0;
public:
    static constexpr int MIN_PRESSURE = 1;
    static constexpr int MAX_PRESSURE = 127;

    virtual ~Aftertouchable() = default;

    std::optional<int> getLastPressure() const { return lastPressure; }

    void aftertouch(int pressure) {
        if (pressure < MIN_PRESSURE || pressure > MAX_PRESSURE)
            throw std::invalid_argument("Aftertouch pressure must be between " + std::to_string(MIN_PRESSURE) + " and " + std::to_string(MAX_PRESSURE));

        lastPressure = pressure;
        onAftertouch(pressure);
    }
};

template <typename T, T MIN, T MAX>
class Continuous {
    T value = MIN;
protected:
    Continuous() = default;
public:
    virtual ~Continuous() = default;

    void setValue(T v) { value = std::clamp(v, MIN, MAX); }
    T getValue() const { return value; }
};

// --- concrete types ---

class Button final : public Component, public Pressable {
    const std::string label;
protected:
    void onPress() override final {
        mapper.trigger(mpc::inputlogic::HardwareTranslator::fromButtonPress(label));
    }
    void onRelease() override final {
        mapper.trigger(mpc::inputlogic::HardwareTranslator::fromButtonRelease(label));
    }
public:
    Button(mpc::inputlogic::InputMapper& mapper, std::string labelToUse)
        : Component(mapper), label(std::move(labelToUse)) {}
};

class Pad final : public Component, public VelocitySensitivePressable, public Aftertouchable {
    const int index;
protected:
    void onPressWithVelocity(int velocity) override final {
        aftertouch(velocity);
        mapper.trigger(mpc::inputlogic::HardwareTranslator::fromPadPress(index, velocity));
    }
    void onAftertouch(int pressure) override final {
        mapper.trigger(mpc::inputlogic::HardwareTranslator::fromPadAftertouch(index, pressure));
    }
    void onRelease() override final {}
    void onPress() override final { pressWithVelocity(MAX_VELO); }
public:
    Pad(int indexToUse, mpc::inputlogic::InputMapper& mapper)
        : Component(mapper), index(indexToUse) {}

    int getIndex() const { return index; }
};

class DataWheel final : public Component {
public:
    explicit DataWheel(mpc::inputlogic::InputMapper& mapper) : Component(mapper) {}
    void turn(int steps) { mapper.trigger(mpc::inputlogic::HardwareTranslator::fromDataWheelTurn(steps)); }
};

class Slider final : public Component, public Continuous<int, 0, 127> {
public:
    explicit Slider(mpc::inputlogic::InputMapper& mapper) : Component(mapper) {}
    void moveTo(int value) {
        setValue(value);
        mapper.trigger(mpc::inputlogic::HardwareTranslator::fromSliderMove(value));
    }
};

class Pot final : public Component, public Continuous<int, 0, 100> {
public:
    explicit Pot(mpc::inputlogic::InputMapper& mapper) : Component(mapper) {}
    void moveTo(int value) {
        setValue(value);
        mapper.trigger(mpc::inputlogic::HardwareTranslator::fromButtonPress("pot-move"));
    }
};

} // namespace mpc::hardware2

