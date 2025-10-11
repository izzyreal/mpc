#pragma once

#include <algorithm>
#include <string>
#include <cassert>
#include <stdexcept>
#include <optional>

#include <inputlogic/ClientInputMapper.h>
#include <inputlogic/ClientInput.h>

namespace mpc::hardware2 {

using namespace mpc::inputlogic;

class Component {
protected:
    mpc::inputlogic::ClientInputMapper& mapper;
    explicit Component(mpc::inputlogic::ClientInputMapper& mapperToUse) : mapper(mapperToUse) {}
public:
    virtual ~Component() = default;
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
};

class Led : public Component {
    private:
        const std::string label;
        bool enabled = false;
    public:
        Led(mpc::inputlogic::ClientInputMapper& mapperToUse, const std::string labelToUse) : Component(mapperToUse), label(labelToUse) {}
        void setEnabled(const bool enabledToUse) { enabled = enabledToUse; }
        bool isEnabled() const { return enabled; }
        std::string getLabel() const { return label; }
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
        if (isPressed())
        {
            return;
        }
        setPressed(true);
        onPress();
    }

    void release() {
        if (!isPressed())
        {
            return;
        }
        setPressed(false);
        onRelease();
    }
};

class VelocitySensitivePressable : public Pressable {
    std::optional<int> velocity = std::nullopt;
protected:
    VelocitySensitivePressable() = default;

    void resetVelocity() { velocity = std::nullopt; }

    virtual void onPressWithVelocity(int velocity) = 0;

    void doPressWithVelocity(int velocityToUse)
    {
        if (velocityToUse < MIN_VELO || velocityToUse > MAX_VELO) {
            throw std::invalid_argument("Velocity must be between " + std::to_string(MIN_VELO) + " and " + std::to_string(MAX_VELO));
        }
        velocity = velocityToUse;
        onPressWithVelocity(velocityToUse);
    }
public:
    static constexpr int MIN_VELO = 1;
    static constexpr int MAX_VELO = 127;

    virtual ~VelocitySensitivePressable() = default;

    void pressWithVelocity(int velocityToUse)
    {
        if (isPressed())
        {
            return;
        }
        setPressed(true);
        doPressWithVelocity(velocityToUse);
    }

    std::optional<int> getVelocity() const { return velocity; }
};

class Aftertouchable {
    std::optional<int> pressure = std::nullopt;
protected:
    Aftertouchable() = default;
    virtual void onAftertouch(int pressure) = 0;

    void resetPressure() { pressure = std::nullopt; }

public:
    static constexpr int MIN_PRESSURE = 1;
    static constexpr int MAX_PRESSURE = 127;

    virtual ~Aftertouchable() = default;

    std::optional<int> getPressure() const { return pressure; }

    void aftertouch(int pressureToUse) {
        if (pressureToUse < MIN_PRESSURE || pressureToUse > MAX_PRESSURE)
            throw std::invalid_argument("Aftertouch pressure must be between " + std::to_string(MIN_PRESSURE) + " and " + std::to_string(MAX_PRESSURE));

        pressure = pressureToUse;
        onAftertouch(pressureToUse);
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
        ClientInput action;
        action.type = ClientInput::Type::ButtonPress;
        action.label = label;
        mapper.trigger(action);
    }
    void onRelease() override final {
        ClientInput action;
        action.type = ClientInput::Type::ButtonRelease;
        action.label = label;
        mapper.trigger(action);
    }
public:
    Button(mpc::inputlogic::ClientInputMapper& mapperToUse, std::string labelToUse)
        : Component(mapperToUse), label(std::move(labelToUse)) {}
};

class Pad final : public Component, public VelocitySensitivePressable, public Aftertouchable {
    const int index;
protected:
    void onPressWithVelocity(int velocity) override final {
        ClientInput action;
        action.type = ClientInput::Type::PadPress;
        action.index = index;
        action.value = velocity;
        mapper.trigger(action);
    }
    void onAftertouch(int pressure) override final {
        ClientInput action;
        action.type = ClientInput::Type::PadAftertouch;
        action.index = index;
        action.value = pressure;
        mapper.trigger(action);
    }
    void onRelease() override final {
        resetPressure();
        resetVelocity();
        ClientInput action;
        action.type = ClientInput::Type::PadRelease;
        action.index = index;
        mapper.trigger(action);
    }
    void onPress() override final {
        doPressWithVelocity(MAX_VELO);
    }
public:
    Pad(int indexToUse, mpc::inputlogic::ClientInputMapper& mapperToUse)
        : Component(mapperToUse), index(indexToUse) {}

    int getIndex() const { return index; }
};

class DataWheel final : public Component {
public:
    explicit DataWheel(mpc::inputlogic::ClientInputMapper& mapperToUse) : Component(mapperToUse) {}
    void turn(int steps) {
        ClientInput action;
        action.type = ClientInput::Type::DataWheelTurn;
        action.value = steps;
        mapper.trigger(action);
    }
};

class Slider final : public Component, public Continuous<int, 0, 127> {
public:
    explicit Slider(mpc::inputlogic::ClientInputMapper& mapperToUse) : Component(mapperToUse) {}
    void moveTo(int value) {
        setValue(value);
        ClientInput action;
        action.type = ClientInput::Type::SliderMove;
        action.value = value;
        mapper.trigger(action);
    }
};

class Pot final : public Component, public Continuous<int, 0, 127> {
public:
    explicit Pot(mpc::inputlogic::ClientInputMapper& mapperToUse) : Component(mapperToUse) {}
    void moveTo(int value) {
        setValue(value);
        ClientInput action;
        action.type = ClientInput::Type::PotMove;
        action.value = value;
        mapper.trigger(action);
    }
};
} // namespace mpc::hardware2
