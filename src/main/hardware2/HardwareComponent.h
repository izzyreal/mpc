#pragma once

#include <algorithm>
#include <string>
#include <cassert>
#include <stdexcept>

namespace mpc::hardware2 {

class Component {
public:
    virtual ~Component() = default;
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
protected:
    Component() = default;
};

class PressState {
    private:
        bool pressed = false;
        PressState(const PressState&) = delete;
        PressState& operator=(const PressState&) = delete;
    protected:
        PressState() = default;
        void setPressed(const bool pressedToUse) { pressed = pressedToUse; }
    public:
        ~PressState() = default;
        bool isPressed() const { return pressed; }
};

class Pressable : public PressState {
    private:
        Pressable(const Pressable&) = delete;
        Pressable& operator=(const Pressable&) = delete;
    protected:
        Pressable() = default;
        virtual void onPress() = 0;
        virtual void onRelease() = 0;
    public:
        virtual ~Pressable() = default;

        void press()
        {
            setPressed(true);
            onPress();
        }

        void release()
        {
            setPressed(false);
            onRelease();
        }
};

class VelocitySensitivePressable : public Pressable {
    private:
        std::optional<int> lastVelocity;
        VelocitySensitivePressable(const VelocitySensitivePressable&) = delete;
        VelocitySensitivePressable& operator=(const VelocitySensitivePressable&) = delete;
    protected:
        VelocitySensitivePressable() = default;
        virtual void onPressWithVelocity(int velocity) = 0;
    public:
        static constexpr int MIN_VELO = 1;
        static constexpr int MAX_VELO = 127;

        virtual ~VelocitySensitivePressable() = default;

        void pressWithVelocity(int velocity)
        {
            if (velocity < MIN_VELO || velocity > MAX_VELO)
            {
                throw std::invalid_argument("Velocity must be between " + std::to_string(MIN_VELO) + " and " + std::to_string(MAX_VELO));
            }

            setPressed(true);
            lastVelocity = velocity;
            onPressWithVelocity(velocity);
        }

        std::optional<int> getLastVelocity() const { return lastVelocity; }
};

class Aftertouchable {
    private:
        std::optional<int> lastPressure;
        Aftertouchable(const Aftertouchable&) = delete;
        Aftertouchable& operator=(const Aftertouchable&) = delete;
    protected:
        Aftertouchable() = default;
        virtual void onAftertouch(int pressure) = 0;
    public:
        static constexpr int MIN_PRESSURE = 1;
        static constexpr int MAX_PRESSURE = 127;

        virtual ~Aftertouchable() = default;

        std::optional<int> getLastPressure() const { return lastPressure; }

        void aftertouch(const int pressure)
        {
            if (pressure < MIN_PRESSURE|| pressure > MAX_PRESSURE)
            {
                throw std::invalid_argument("Aftertouch pressure must be between " + std::to_string(MIN_PRESSURE) + " and " + std::to_string(MAX_PRESSURE));
            }

            lastPressure = pressure;
            onAftertouch(pressure);
        }
};

template <typename T, T MIN, T MAX>
class Continuous {
    private:
        Continuous(const Continuous&) = delete;
        Continuous& operator=(const Continuous&) = delete;
    public:
        virtual ~Continuous() = default;
        void setValue(const T v)
        {
            value = std::clamp(v, MIN, MAX);
        }

        T getValue() const
        {
            return value;
        }

    protected:
        Continuous() = default;
        T value = MIN;
};

// --- concrete types ---

class Button final : public Component, public Pressable {
    private:
        Button(const Button&) = delete;
        Button& operator=(const Button&) = delete;
    protected:
        void onPress() override final { }
        void onRelease() override final { }
    public:
        Button() = default;
};

class Pad final : public Component, public VelocitySensitivePressable, public Aftertouchable {
    private:
        Pad(const Pad&) = delete;
        Pad& operator=(const Pad&) = delete;
    protected:
        void onPressWithVelocity(int velocity) override final
        {
            aftertouch(velocity);
        }
        void onAftertouch(int pressure) override final { }
        void onRelease() override final { }
        void onPress() override final { pressWithVelocity(MAX_VELO); }
    public:
        Pad() = default;
};

class DataWheel final : public Component {
    private:
        DataWheel(const DataWheel&) = delete;
        DataWheel& operator=(const DataWheel&) = delete;
    public:
        void turn(int steps) { }
        DataWheel() = default;
};

class Slider final : public Component, public Continuous<int, 0, 127> {
    private:
        Slider(const Slider&) = delete;
        Slider& operator=(const Slider&) = delete;
    public:
        Slider() = default;
};

class Pot final : public Component, public Continuous<int, 0, 100> {
    private:
        Pot(const Pot&) = delete;
        Pot& operator=(const Pot&) = delete;
    public:
        Pot() = default;
};

} // namespace mpc::hardware2

