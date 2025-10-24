#pragma once

#include "Component.hpp"

namespace mpc::lcdgui
{

    class Effect
        : public Component
    {

    private:
        bool filled = false;

    public:
        void setFilled(bool b);
        void Draw(std::vector<std::vector<bool>> *pixels) override;
        Effect(MRECT rect);
    };

} // namespace mpc::lcdgui
