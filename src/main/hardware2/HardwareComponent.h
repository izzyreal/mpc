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

template <typename T, int MIN, int MAX>
class Continuous {
    T value = static_cast<T>(MIN);
protected:
    Continuous() = default;
    
public:
    virtual ~Continuous() = default;

    void setValue(T v) { value = std::clamp(v, static_cast<T>(MIN), static_cast<T>(MAX)); }
    T getValue() const { return value; }

    std::pair<int, int> getRange() const { return { MIN, MAX }; }

    template <typename T1>
    std::pair<T1, T1> getRangeAs() const { return { static_cast<T1>(MIN), static_cast<T1>(MAX) }; }

    template <typename T1>
    T1 getValueAs() const
    {
        if constexpr (std::is_floating_point_v<T> && std::is_integral_v<T1>)
        {
            return static_cast<T1>(std::lround(value));
        }
        else
        {
            return static_cast<T1>(value);
        }
    }
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

// Modeled with float value to support smooth GUI experience.
// With an integer type we run into the problem that dragging the slider may
// well be of a higher resolution than 128 discrete values, leading to a visible
// jump on the first occasion that the GUI slider synchronizes with the model state.
class Slider final : public Component, public Continuous<float, 0, 127> {
public:
    enum class Direction { UpIncreases, DownIncreases };
private:
    Direction direction;
public:
    explicit Slider(mpc::inputlogic::ClientInputMapper& mapperToUse)
        : Component(mapperToUse),
        // According to https://www.mpc-forums.com/viewtopic.php?p=790276#p790276
        // the MPC2000XL's slider has value 127 at the top and 0 at the bottom.
        // We explicitly model the direction, because it has been a frequent source
        // of confusion for the author of VMPC2000XL.
        direction(Direction::UpIncreases)
    {
    }
    
    // Moves the slider based on a normalized GUI Y position in [0.0, 1.0],
    // where 0.0 represents the top of the slider GUI representation, and 1.0
    // the bottom.
    // Internally converts this visual Y position into the corresponding
    // hardware value (0–127) according to the slider's direction.
    void moveToNormalizedY(const float normalizedY)
    {
        auto [min, max] = getRangeAs<float>();

        const float clampedY = std::clamp(normalizedY, 0.0f, 1.0f);

        float value;

        if (direction == Direction::UpIncreases)
        {
            value = (1.0f - clampedY) * max + clampedY * min;
        }
        else
        {
            value = clampedY * max + (1.0f - clampedY) * min;
        }

        moveTo(value);
    }

    void moveTo(const float value)
    {
        setValue(value);
        ClientInput action;
        action.type = ClientInput::Type::SliderMove;
        action.value = getValueAs<int>();
        mapper.trigger(action);
    }

    void setDirection(const Direction directionToUse)
    {
        direction = directionToUse;
    }

    Direction getDirection() const { return direction; }
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
