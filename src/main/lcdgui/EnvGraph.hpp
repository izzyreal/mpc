#pragma once
#include "Component.hpp"

#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui
{
    class EnvGraph : public Component
    {

        Mpc &mpc;
        std::vector<std::vector<int>> coordinates;

    public:
        void setCoordinates(const std::vector<std::vector<int>> &ia);
        void setCoordinates(int attack, int decay, bool decayModeStart);

        void Draw(std::vector<std::vector<bool>> *pixels) override;

        EnvGraph(Mpc &mpc);
    };
} // namespace mpc::lcdgui
