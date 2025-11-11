#pragma once
#include "Component.hpp"

#include <vector>

namespace mpc::lcdgui
{
    class Underline : public Component
    {

        std::vector<bool> states;

    public:
        void Draw(std::vector<std::vector<bool>> *pixels) override;

        void setState(int i, bool b);

        Underline();
    };
} // namespace mpc::lcdgui
