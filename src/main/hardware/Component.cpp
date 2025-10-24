#include "hardware/Component.hpp"
#include <cassert>

using namespace mpc::hardware;

Component::Component(ComponentId idToUse) : id(idToUse) {}

Component::~Component() = default;

ComponentId Component::getId() const { return id; }

PressState::PressState() = default;

PressState::~PressState() = default;

void PressState::setPressed(bool pressedToUse) { pressed = pressedToUse; }

bool PressState::isPressed() const { return pressed; }

Pressable::Pressable() = default;

Pressable::~Pressable() = default;

void Pressable::onPress() {}

void Pressable::onRelease() {}

bool Pressable::press()
{
    if (isPressed())
    {
        return false;
    }

    setPressed(true);
    onPress();
    return true;
}

void Pressable::release()
{
    if (!isPressed())
    {
        return;
    }

    setPressed(false);
    onRelease();
}

bool Pressable::doublePress() { return press(); }

VelocitySensitivePressable::VelocitySensitivePressable() = default;

VelocitySensitivePressable::~VelocitySensitivePressable() = default;

void VelocitySensitivePressable::resetVelocity() { velocity = std::nullopt; }

void VelocitySensitivePressable::doPressWithVelocity(int velocityToUse)
{
    if (velocityToUse < MIN_VELO || velocityToUse > MAX_VELO)
    {
        throw std::invalid_argument("Velocity must be between " +
                                    std::to_string(MIN_VELO) + " and " +
                                    std::to_string(MAX_VELO));
    }

    velocity = velocityToUse;
}

bool VelocitySensitivePressable::pressWithVelocity(int velocityToUse)
{
    if (isPressed())
    {
        return false;
    }

    setPressed(true);
    doPressWithVelocity(velocityToUse);
    return true;
}

std::optional<int> VelocitySensitivePressable::getVelocity() const { return velocity; }

Aftertouchable::Aftertouchable() = default;

Aftertouchable::~Aftertouchable() = default;

void Aftertouchable::onAftertouch(int) {}

void Aftertouchable::resetPressure() { pressure = std::nullopt; }

std::optional<int> Aftertouchable::getPressure() const { return pressure; }

void Aftertouchable::aftertouch(int pressureToUse)
{
    if (pressureToUse < MIN_PRESSURE || pressureToUse > MAX_PRESSURE)
    {
        throw std::invalid_argument("Aftertouch pressure must be between " +
                                    std::to_string(MIN_PRESSURE) + " and " +
                                    std::to_string(MAX_PRESSURE));
    }

    pressure = pressureToUse;
    onAftertouch(pressureToUse);
}

Led::Led(ComponentId id) : Component(id) {}

void Led::setEnabled(bool enabledToUse) { enabled = enabledToUse; }

bool Led::isEnabled() const { return enabled; }

Button::Button(ComponentId id) : Component(id) {}

Pad::Pad(int indexToUse)
    : Component(static_cast<ComponentId>(ComponentId::PAD_1_OR_AB + indexToUse)),
      index(indexToUse)
{
    assert(index >= 0 && index < 16);
}

void Pad::onRelease()
{
    resetPressure();
    resetVelocity();
}

void Pad::onPress() { doPressWithVelocity(MAX_VELO); }

int Pad::getIndex() const { return index; }

DataWheel::DataWheel() : Component(ComponentId::DATA_WHEEL) {}

void DataWheel::turn(int steps)
{
    angle = angle + (angleIncrementPerStep * (float)std::clamp(steps, -20, 20));

    if (angle > 100.0f)
    {
        angle -= std::floor(angle);
    }
    else if (angle < -100.0f)
    {
        angle -= std::floor(angle);
    }
}

float DataWheel::getAngle() { return angle; }

Slider::Slider() : Component(ComponentId::SLIDER), direction(Direction::UpIncreases) {}

void Slider::moveToNormalizedY(float normalizedY)
{
    auto [min, max] = getRangeAs<float>();
    const float clampedY = std::clamp(normalizedY, 0.0f, 1.0f);
    float value = (direction == Direction::UpIncreases)
        ? (1.0f - clampedY) * max + clampedY * min
        : clampedY * max + (1.0f - clampedY) * min;
    setValue(value);
}

void Slider::setDirection(Direction directionToUse) { direction = directionToUse; }

Slider::Direction Slider::getDirection() const { return direction; }

Pot::Pot(ComponentId id) : Component(id) {}

template <typename T, int MIN, int MAX>
void Continuous<T, MIN, MAX>::setValue(T v)
{
    value = std::clamp(v, static_cast<T>(MIN), static_cast<T>(MAX));
}

template <typename T, int MIN, int MAX>
T Continuous<T, MIN, MAX>::getValue() const { return value; }

template <typename T, int MIN, int MAX>
std::pair<int, int> Continuous<T, MIN, MAX>::getRange() const { return {MIN, MAX}; }

template <typename T, int MIN, int MAX>
template <typename T1>
std::pair<T1, T1> Continuous<T, MIN, MAX>::getRangeAs() const
{
    return {static_cast<T1>(MIN), static_cast<T1>(MAX)};
}

template class mpc::hardware::Continuous<float, 0, 127>;
template class mpc::hardware::Continuous<float, 0, 1>;
 
