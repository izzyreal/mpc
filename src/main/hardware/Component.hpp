#pragma once

#include "ComponentId.hpp"

#include <optional>
#include <utility>

namespace mpc::hardware
{

    class Component
    {
    public:
        virtual ~Component();
        Component(const Component &) = delete;
        Component &operator=(const Component &) = delete;
        ComponentId getId() const;

    protected:
        explicit Component(ComponentId idToUse);

    private:
        const ComponentId id;
    };

    class PressState
    {
    private:
        bool pressed = false;

    protected:
        PressState();
        void setPressed(bool pressedToUse);

    public:
        virtual ~PressState();
        bool isPressed() const;
    };

    class Pressable : public PressState
    {
    protected:
        Pressable();
        virtual void onPress();
        virtual void onRelease();

    public:
        virtual ~Pressable();
        bool press();
        void release();
        bool doublePress();
    };

    class VelocitySensitivePressable : public Pressable
    {
    private:
        std::optional<int> velocity = std::nullopt;

    protected:
        VelocitySensitivePressable();
        void resetVelocity();
        void doPressWithVelocity(int velocityToUse);

    public:
        static constexpr int MIN_VELO = 1;
        static constexpr int MAX_VELO = 127;
        virtual ~VelocitySensitivePressable();
        bool pressWithVelocity(int velocityToUse);
        std::optional<int> getVelocity() const;
    };

    class Aftertouchable
    {
    private:
        std::optional<int> pressure = std::nullopt;

    protected:
        Aftertouchable();
        virtual void onAftertouch(int pressure);
        void resetPressure();

    public:
        static constexpr int MIN_PRESSURE = 1;
        static constexpr int MAX_PRESSURE = 127;
        virtual ~Aftertouchable();
        std::optional<int> getPressure() const;
        void aftertouch(int pressureToUse);
    };

    template <typename T, int MIN, int MAX> class Continuous
    {
    private:
        T value = static_cast<T>(MIN);

    protected:
        Continuous() = default;

    public:
        virtual ~Continuous() = default;
        void setValue(T v);
        T getValue() const;
        std::pair<int, int> getRange() const;

        template <typename T1> std::pair<T1, T1> getRangeAs() const;

        template <typename T1> T1 getValueAs() const
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

    class Led : public Component
    {
    private:
        bool enabled = false;

    public:
        explicit Led(ComponentId id);
        void setEnabled(bool enabledToUse);
        bool isEnabled() const;
    };

    class Button final : public Component, public Pressable
    {
    public:
        explicit Button(ComponentId id);
    };

    class Pad final : public Component,
                      public VelocitySensitivePressable,
                      public Aftertouchable
    {
    private:
        const int index;

    protected:
        void onRelease() override;
        void onPress() override;

    public:
        explicit Pad(int indexToUse);
        int getIndex() const;
    };

    class DataWheel final : public Component
    {
    private:
        const int STEP_COUNT_FOR_360_DEGREES = 100;
        const float angleIncrementPerStep =
            1.f / (float)STEP_COUNT_FOR_360_DEGREES;
        float angle = 0.f;

    public:
        explicit DataWheel();
        void turn(int steps);
        float getAngle();
    };

    class Slider final : public Component, public Continuous<float, 0, 127>
    {
    public:
        enum class Direction
        {
            UpIncreases,
            DownIncreases
        };

    private:
        Direction direction;

    public:
        explicit Slider();
        void moveToNormalizedY(float normalizedY);
        void setDirection(Direction directionToUse);
        Direction getDirection() const;
    };

    class Pot final : public Component, public Continuous<float, 0, 1>
    {
    public:
        explicit Pot(ComponentId id);
    };

} // namespace mpc::hardware
