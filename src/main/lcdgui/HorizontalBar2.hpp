#pragma once
#include "Component.hpp"

namespace mpc::lcdgui
{
    class HorizontalBar2 : public Component
    {

    private:
        float value = 0.0f;

    public:
        void setValue(float value);
        void Draw(std::vector<std::vector<bool>> *pixels) override;

        HorizontalBar2(MRECT rect);
    };
} // namespace mpc::lcdgui
