#pragma once
#include "Component.hpp"

#include <vector>

namespace mpc::lcdgui
{
    class Underline : public Component
    {

    private:
        std::vector<bool> states;

    public:
        void Draw(std::vector<std::vector<bool>> *pixels) override;

    public:
        void setState(int i, bool b);

    public:
        Underline();
    };
} // namespace mpc::lcdgui
