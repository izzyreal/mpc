#pragma once

#include <algorithm>
#include <string>
#include <cassert>
#include <stdexcept>
#include <optional>

namespace mpc::hardware2 {

class Component {
public:
    virtual ~Component() = default;
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
protected:
    Component() = default;
};

class Led : public Component {
private:
    const std::string label;
    bool enabled = false;
public:
    Led(const std::string labelToUse) : label(labelToUse) {}
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
    virtual void onPress() {}
    virtual void onRelease() {}
public:
    virtual ~Pressable() = default;

    bool press() {
        if (isPressed()) return false;
        setPressed(true);
        onPress();
        return true;
    }

    void release() {
        if (!isPressed()) return;
        setPressed(false);
        onRelease();
    }

    bool doublePress() {
        return press();
    }
};

class VelocitySensitivePressable : public Pressable {
    std::optional<int> velocity = std::nullopt;
protected:
    VelocitySensitivePressable() = default;
    void resetVelocity() { velocity = std::nullopt; }
    void doPressWithVelocity(int velocityToUse) {
        if (velocityToUse < MIN_VELO || velocityToUse > MAX_VELO) {
            throw std::invalid_argument("Velocity must be between " + std::to_string(MIN_VELO) + " and " + std::to_string(MAX_VELO));
        }
        velocity = velocityToUse;
    }
public:
    static constexpr int MIN_VELO = 1;
    static constexpr int MAX_VELO = 127;
    virtual ~VelocitySensitivePressable() = default;

    bool pressWithVelocity(int velocityToUse) {
        if (isPressed()) return false;
        setPressed(true);
        doPressWithVelocity(velocityToUse);
        return true;
    }

    std::optional<int> getVelocity() const { return velocity; }
};

class Aftertouchable {
    std::optional<int> pressure = std::nullopt;
protected:
    Aftertouchable() = default;
    virtual void onAftertouch(int pressure) {}
    void resetPressure() { pressure = std::nullopt; }
public:
    static constexpr int MIN_PRESSURE = 1;
    static constexpr int MAX_PRESSURE = 127;
    virtual ~Aftertouchable() = default;

    std::optional<int> getPressure() const { return pressure; }

    void aftertouch(int pressureToUse) {
        if (pressureToUse < MIN_PRESSURE || pressureToUse > MAX_PRESSURE) {
            throw std::invalid_argument("Aftertouch pressure must be between " + std::to_string(MIN_PRESSURE) + " and " + std::to_string(MAX_PRESSURE));
        }
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
    T1 getValueAs() const {
        if constexpr (std::is_floating_point_v<T> && std::is_integral_v<T1>) {
            return static_cast<T1>(std::lround(value));
        } else {
            return static_cast<T1>(value);
        }
    }
};

// --- concrete types ---

class Button final : public Component, public Pressable {
    const std::string label;
public:
    explicit Button(const std::string labelToUse) : label(std::move(labelToUse)) {}
    std::string getLabel() const { return label; }
};

class Pad final : public Component, public VelocitySensitivePressable, public Aftertouchable {
    const int index;
protected:
    void onRelease() override final { resetPressure(); resetVelocity(); }
    void onPress() override final { doPressWithVelocity(MAX_VELO); }
public:
    Pad(int indexToUse) : index(indexToUse) {}
    int getIndex() const { return index; }
};

class DataWheel final : public Component {
private:
    const int STEP_COUNT_FOR_360_DEGREES = 100;
    const float angleIncrementPerStep = 1.f / STEP_COUNT_FOR_360_DEGREES;

    // The normalized current angle of the DATA wheel. Only affects GUI representation.
    // 0 means the dimple is at the top, 0.25 at 90 degrees clockwise, 0.5 at 180 degrees, 0.75 at 270 degrees clockwise
    float angle = 0.f;
public:
    explicit DataWheel() = default;
    void turn(const int steps)
    {
        angle = std::fmod(angle + (angleIncrementPerStep * steps), 1.f);
    }
    float getAngle() { return angle; }
};

class Slider final : public Component, public Continuous<float, 0, 127> {
public:
    enum class Direction { UpIncreases, DownIncreases };
private:
    Direction direction;
public:
    explicit Slider() : direction(Direction::UpIncreases) {}

    void moveToNormalizedY(const float normalizedY) {
        auto [min, max] = getRangeAs<float>();
        const float clampedY = std::clamp(normalizedY, 0.0f, 1.0f);
        float value = (direction == Direction::UpIncreases)
            ? (1.0f - clampedY) * max + clampedY * min
            : clampedY * max + (1.0f - clampedY) * min;
        moveTo(value);
    }

    void moveTo(const float value) {
        setValue(value);
    }

    void setDirection(const Direction directionToUse) { direction = directionToUse; }
    Direction getDirection() const { return direction; }
};

class Pot final : public Component, public Continuous<int, 0, 127> {
public:
    explicit Pot() {}
    void moveTo(int value) { setValue(value); }
};

} // namespace mpc::hardware2
