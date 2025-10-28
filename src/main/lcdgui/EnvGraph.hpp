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

    private:
        mpc::Mpc &mpc;
        std::vector<std::vector<int>> coordinates;

    public:
        void setCoordinates(std::vector<std::vector<int>> ia);
        void setCoordinates(int attack, int decay, bool decayModeStart);

    public:
        void Draw(std::vector<std::vector<bool>> *pixels) override;

    public:
        EnvGraph(mpc::Mpc &mpc);
    };
} // namespace mpc::lcdgui
